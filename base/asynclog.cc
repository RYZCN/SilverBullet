
#include "asynclog.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include "logfile.h"
using namespace std;
AsyncLogging::AsyncLogging(std::string logFileName_, int flushInterval)
    : flushInterval_(flushInterval),
      is_running_(false),
      name_(logFileName_),
      thread_(),
      curr_buf_(new Buffer()),
      next_buf_(new Buffer()),
      buffers_(),
      latch_(1)
{
    assert(logFileName_.size() > 1);
    curr_buf_->init_zero();
    next_buf_->init_zero();
    buffers_.reserve(16);
}
AsyncLogging::~AsyncLogging()
{
    if (is_running_)
        stop();
}
void AsyncLogging::append(const char *logline, int len)
{
    unique_lock lock(mutex_);
    if (curr_buf_->rest() > len)
        curr_buf_->append(logline, len);
    else
    {
        buffers_.push_back(curr_buf_);
        curr_buf_.reset();
        if (next_buf_)
            curr_buf_ = std::move(next_buf_);
        else
            curr_buf_.reset(new Buffer);
        curr_buf_->append(logline, len);
        cond_.notify_one();
    }
}

void AsyncLogging::threadFunc()
{
    assert(is_running_ == true);
    latch_.count_down();
    LogFile output(name_);

    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->init_zero();
    newBuffer2->init_zero();

    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while (is_running_)
    {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        {
            unique_lock lock(mutex_);
            if (buffers_.empty())
            {
                cond_.wait_for(lock, chrono::duration<int>(flushInterval_));
            }
            buffers_.push_back(curr_buf_);
            curr_buf_.reset();

            curr_buf_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if (!next_buf_)
            {
                next_buf_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        if (buffersToWrite.size() > 25)
        {
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }

        for (size_t i = 0; i < buffersToWrite.size(); ++i)
        {
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        }

        if (buffersToWrite.size() > 2)
        {
            buffersToWrite.resize(2);
        }

        if (!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }
        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}
void AsyncLogging::start()
{
    is_running_ = true;
    thread_ = thread(std::bind(&AsyncLogging::threadFunc, this));
    latch_.wait();
}
void AsyncLogging::stop()
{
    is_running_ = false;
    cond_.notify_one();
    thread_.join();
}