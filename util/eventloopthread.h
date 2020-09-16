#ifndef SILVERBULLET_UTIL_EVENTLOOPTHREAD_H_
#define SILVERBULLET_UTIL_EVENTLOOPTHREAD_H_
#include "eventloop.h"
#include "noncopyable.h"
#include <condition_variable>
#include <memory>
#include <thread>
namespace SB
{
    namespace util
    {

        class EventLoopThread : noncopyable
        {
        public:
            EventLoopThread();
            ~EventLoopThread();
            EventLoop *start();

        private:
            void threadFunc();
            EventLoop *loop_;
            bool exiting_;
            std::thread thread_;
            std::mutex mutex_;
            std::condition_variable cond_;
        };
    } // namespace util

} // namespace SB

#endif