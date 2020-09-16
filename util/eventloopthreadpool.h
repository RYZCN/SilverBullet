#ifndef SILVERBULLET_UTIL_EVENTLOOPTHREADPOOL_H_
#define SILVERBULLET_UTIL_EVENTLOOPTHREADPOOL_H_
#include "eventloopthread.h"
#include "logger.h"
#include "noncopyable.h"
#include <memory>
#include <vector>
namespace SB
{
    namespace util
    {

        class EventLoopThreadPool : noncopyable
        {
        public:
            EventLoopThreadPool(EventLoop *baseLoop, int numThreads);

            ~EventLoopThreadPool()
            {
                LOG << "~EventLoopThreadPool()";
            }
            void start();
            EventLoop *next_loop();

        private:
            EventLoop *baseLoop_;
            bool started_;
            int numThreads_;
            int next_;
            std::vector<std::shared_ptr<EventLoopThread>> threads_;
            std::vector<EventLoop *> loops_;
        };
    } // namespace util
} // namespace SB

#endif