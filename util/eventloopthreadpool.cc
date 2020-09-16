
#include "eventloopthreadpool.h"

namespace SB
{
    namespace util
    {
        EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, int numThreads)
            : baseLoop_(baseLoop), started_(false), numThreads_(numThreads), next_(0)
        {
            if (numThreads_ <= 0)
            {
                LOG << "numThreads_ <= 0";
                abort();
            }
        }

        void EventLoopThreadPool::start()
        {
            started_ = true;
            for (int i = 0; i < numThreads_; ++i)
            {
                std::shared_ptr<EventLoopThread> t(new EventLoopThread());
                threads_.push_back(t);
                loops_.push_back(t->start());
            }
        }

        EventLoop *EventLoopThreadPool::next_loop() //分发
        {

            assert(started_);

            EventLoop *loop = baseLoop_;
            if (!loops_.empty())
            {
                loop = loops_[next_];
                next_ = (next_ + 1) % numThreads_;
            }
            return loop;
        }

    } // namespace util

} // namespace SB
