#include "asynclog.h"
#include "logfile.h"
#include <assert.h>
#include <functional>
#include <stdio.h>
#include <unistd.h>
using namespace std;
AsyncLog::AsyncLog(std::string logFileName_, int flushInterval)
    : flush_interval_(flushInterval),
      is_running_(false),
      name_(logFileName_),
      back_thread_(),
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
AsyncLog::~AsyncLog()
{
    if (is_running_)
        stop();
}
void AsyncLog::append(const char *logline, int len)
{
    unique_lock<mutex> lock(mutex_); //单线程
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
        buf_to_write_cond_.notify_one();
    }
}

void AsyncLog::back_up_func()
{
    assert(is_running_ == true);
    latch_.count_down();
    LogFile output(name_);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->init_zero();
    newBuffer2->init_zero();

    BufferVector buf_to_wirte;
    buf_to_wirte.reserve(16);
    while (is_running_)
    {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buf_to_wirte.empty());

        {
            unique_lock<mutex> lock(mutex_);
            if (buffers_.empty())
            {
                Seconds wait_time(flush_interval_);
                buf_to_write_cond_.wait_for(lock, wait_time);
            }
            buffers_.push_back(curr_buf_);
            curr_buf_.reset();

            curr_buf_ = std::move(newBuffer1);
            buf_to_wirte.swap(buffers_);
            if (!next_buf_)
            {
                next_buf_ = std::move(newBuffer2);
            }
        }

        assert(!buf_to_wirte.empty());
        if (buf_to_wirte.size() > 25)
        {
            buf_to_wirte.erase(buf_to_wirte.begin() + 2, buf_to_wirte.end());
        }
        for (size_t i = 0; i < buf_to_wirte.size(); ++i)
        {
            output.append(buf_to_wirte[i]->data(), buf_to_wirte[i]->length());
        }

        if (buf_to_wirte.size() > 2)
        {
            buf_to_wirte.resize(2);
        }

        if (!newBuffer1)
        {
            assert(!buf_to_wirte.empty());
            newBuffer1 = buf_to_wirte.back();
            buf_to_wirte.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2)
        {
            assert(!buf_to_wirte.empty());
            newBuffer2 = buf_to_wirte.back();
            buf_to_wirte.pop_back();
            newBuffer2->reset();
        }
        buf_to_wirte.clear();
        output.flush();
    }
    output.flush();
}
void AsyncLog::start()
{
    is_running_ = true;
    back_thread_ = thread(std::bind(&AsyncLog::back_up_func, this));
    latch_.wait();
}
void AsyncLog::stop()
{
    is_running_ = false;
    buf_to_write_cond_.notify_one();
    back_thread_.join();
}