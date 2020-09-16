#ifndef SILVERBULLET_UTIL_EPOLLER_H_
#define SILVERBULLET_UTIL_EPOLLER_H_
#include "channel.h"
#include "httpdata.h"
#include "noncopyable.h"
#include "timer.h"
#include <condition_variable>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <sys/epoll.h>
#include <thread>
#include <vector>
namespace SB
{
    namespace util
    {
        class Channel;
        class HttpMessage;
        class Epoller
        {
        public:
            typedef std::shared_ptr<Channel> SP_Channel;
            Epoller();
            ~Epoller();
            void epoll_add(SP_Channel request, int timeout);
            void epoll_mod(SP_Channel request, int timeout);
            void epoll_del(SP_Channel request);
            std::vector<std::shared_ptr<Channel>> poll();
            std::vector<std::shared_ptr<Channel>> get_return_events(int events_num);

            int get_fd() { return epollFd_; }
            void handle_expired();

        private:
            void add_timer(std::shared_ptr<Channel> request_data, int timeout);
            static const int MAXFDS = 100000;
            int epollFd_;
            std::vector<epoll_event> events_;
            std::shared_ptr<Channel> fd2chan_[MAXFDS];
            std::shared_ptr<HttpMessage> fd2http_[MAXFDS];
            TimerManager timerManager_; //每个Epoller一个定时器管理单元
        };
    } // namespace util
} // namespace SB

#endif