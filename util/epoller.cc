#include "epoller.h"
#include "logger.h"
#include "util.h"
#include <assert.h>
#include <deque>
#include <errno.h>
#include <netinet/in.h>
#include <queue>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <iostream>
using namespace std;

namespace SB
{
    namespace util
    {
        const int EVENTSNUM = 4096;
        const int EPOLLWAIT_TIME = 10000;
        using namespace std;

        typedef shared_ptr<Channel> SP_Channel;

        Epoller::Epoller() : epollFd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSNUM)
        {
            assert(epollFd_ > 0);
        }
        Epoller::~Epoller() {}

        // 注册新描述符
        void Epoller::epoll_add(SP_Channel request, int timeout)
        {
            int fd = request->get_fd();
            if (timeout > 0)
            {
                add_timer(request, timeout);
                fd2http_[fd] = request->get_holder();
            }
            struct epoll_event event;

            event.data.fd = fd;
            event.events = request->get_events();

            request->update_event_and_judge_is_same();

            fd2chan_[fd] = request;
            if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0)
            {
                perror("epoll_add error");
                fd2chan_[fd].reset();
            }
        }

        // 修改描述符状态
        void Epoller::epoll_mod(SP_Channel request, int timeout)
        {
            if (timeout > 0)
                add_timer(request, timeout);
            int fd = request->get_fd();
            if (!request->update_event_and_judge_is_same())
            {
                struct epoll_event event;
                event.data.fd = fd;
                event.events = request->get_events();
                if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0)
                {
                    perror("epoll_mod error");
                    fd2chan_[fd].reset();
                }
            }
        }

        // 从epoll中删除描述符
        void Epoller::epoll_del(SP_Channel request)
        {
            int fd = request->get_fd();
            struct epoll_event event;
            event.data.fd = fd;
            event.events = request->get_last_events();
            // event.events = 0;
            // request->EqualAndUpdateLastEvents()
            if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0)
            {
                perror("epoll_del error");
            }
            fd2chan_[fd].reset();
            fd2http_[fd].reset();
        }

        // 返回活跃事件数
        std::vector<SP_Channel> Epoller::poll()
        {
            while (true)
            {
                int event_count =
                    epoll_wait(epollFd_, &*events_.begin(), events_.size(), EPOLLWAIT_TIME);
                if (event_count < 0)
                    perror("epoll wait error");
                std::vector<SP_Channel> req_data = get_return_events(event_count);
                if (req_data.size() > 0)
                    return req_data;
            }
        }

        void Epoller::handle_expired() { timerManager_.handle_expired(); }

        std::vector<SP_Channel> Epoller::get_return_events(int events_num)
        {
            std::vector<SP_Channel> req_data;
            for (int i = 0; i < events_num; ++i)
            {
                // 获取有事件产生的描述符
                int fd = events_[i].data.fd;

                SP_Channel cur_req = fd2chan_[fd];

                if (cur_req)
                {
                    cur_req->set_revents(events_[i].events);
                    cur_req->set_events(0);
                    req_data.push_back(cur_req);
                }
                else
                {
                    LOG << "SP cur_req is invalid";
                }
            }
            return req_data;
        }

        void Epoller::add_timer(SP_Channel request_data, int timeout)
        {
            shared_ptr<HttpMessage> t = request_data->get_holder();
            if (t)
                timerManager_.addTimer(t, timeout);
            else
                ;
            LOG << "timer add fail";
        }
    } // namespace util

} // namespace SB
