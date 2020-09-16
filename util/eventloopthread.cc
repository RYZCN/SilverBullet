#include "eventloopthread.h"
#include <functional>

namespace SB
{
    namespace util
    {

        EventLoopThread::EventLoopThread()
            : loop_(NULL),
              exiting_(false),
              thread_(),
              mutex_()
        {
        }

        EventLoopThread::~EventLoopThread()
        {
            exiting_ = true;
            if (loop_ != NULL)
            {
                loop_->quit();
                thread_.join();
            }
        }

        EventLoop *EventLoopThread::start()
        {
            thread_ = thread(bind(&EventLoopThread::threadFunc, this));
            {
                unique_lock<mutex> lock(mutex_);
                while (loop_ == NULL)
                    cond_.wait(lock);
            }
            return loop_;
        }

        void EventLoopThread::threadFunc()
        {
            EventLoop loop;
            {
                unique_lock<mutex> lock(mutex_);
                loop_ = &loop;
                cond_.notify_one();
            }
            loop.loop();
            loop_ = NULL;
        }
    } // namespace util

} // namespace SB
