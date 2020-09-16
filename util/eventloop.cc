#include "eventloop.h"
#include "logger.h"
#include "util.h"
#include <iostream>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
using namespace std;
namespace SB
{
    namespace util
    {
        __thread EventLoop *t_loopInThisThread = 0; //线程局部变量

        int createEventfd()
        {
            int event_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
            if (event_fd < 0)
            {
                LOG << "Failed in eventfd";
                abort();
            }
            return event_fd;
        }

        EventLoop::EventLoop()
            : looping_(false),
              poller_(new Epoller()),
              wakeup_fd_(createEventfd()),
              quit_(false),
              eventHandling_(false),
              is_process_task_(false),
              wakeup_channel_(new Channel(this, wakeup_fd_))
        {
            if (t_loopInThisThread)
            {
            }
            else
            {
                t_loopInThisThread = this;
            }
            wakeup_channel_->set_events(EPOLLIN | EPOLLET);
            wakeup_channel_->set_read_handler(bind(&EventLoop::handle_read, this));
            wakeup_channel_->set_env_handler(bind(&EventLoop::handle_env, this));
            poller_->epoll_add(wakeup_channel_, 0);
        }

        void EventLoop::handle_env()
        {
            update(wakeup_channel_, 0);
        }

        EventLoop::~EventLoop()
        {
            close(wakeup_fd_);
            t_loopInThisThread = NULL;
        }

        void EventLoop::wake_up()
        {
            uint64_t one = 1;
            ssize_t n = writen(wakeup_fd_, (char *)(&one), sizeof one);
            if (n != sizeof one)
            {
                LOG << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
            }
        }

        void EventLoop::handle_read()
        {
            uint64_t one = 1;
            ssize_t n = readn(wakeup_fd_, &one, sizeof one);
            if (n != sizeof one)
            {
                LOG << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
            }
            wakeup_channel_->set_events(EPOLLIN | EPOLLET);
        }

        void EventLoop::add_loop_task(Task &&cb)
        {
            put(std::move(cb));
        }

        void EventLoop::put(Task &&cb)
        {
            {
                std::lock_guard<mutex> lock(mutex_);
                tasks_.emplace_back(std::move(cb));
            }
            wake_up();
        }

        void EventLoop::loop()
        {
            assert(!looping_);
            looping_ = true;
            quit_ = false;
            std::vector<SP_Channel> ret;
            while (!quit_)
            {
                ret.clear();
                ret = poller_->poll();
                eventHandling_ = true;
                for (auto &it : ret)
                    it->handle_events();
                eventHandling_ = false;
                process_task();
                poller_->handle_expired();
            }
            looping_ = false;
        }

        void EventLoop::process_task()
        {
            std::vector<Task> ts;
            is_process_task_ = true;
            {
                std::lock_guard<mutex> lock(mutex_);
                ts.swap(tasks_);
            }

            for (size_t i = 0; i < ts.size(); ++i)
                ts[i]();
            is_process_task_ = false;
        }

        void EventLoop::quit()
        {
            quit_ = true;
            wake_up();
        }
    } // namespace util

} // namespace SB
