#ifndef SILVERBULLET_EPOLLER_H_
#define SILVERBULLET_EPOLLER_H_
#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include <deque>
#include <sys/epoll.h>
#include <condition_variable>
#include <map>
#include <base/noncopyable.h>
#include "eventloop.h"
#include "channel.h"
namespace SB
{
    namespace util
    {

        using CallBack = std::function<void()>;
        using ChannelSP = std::shared_ptr<Channel>;
        using EventMap = std::map<int, ChannelSP>;
        /**
         * @brief Epoller类 处理事件 对应一个内核epoll
         * 
         */

        class Epoller : public noncopyable
        {
        public:
            explicit Epoller(EventLoop *owner);
            ~Epoller();

        private:
            EventMap events_;
            EventLoop *owner_;
            int fd_;
        };
    } // namespace util
} // namespace SB

#endif