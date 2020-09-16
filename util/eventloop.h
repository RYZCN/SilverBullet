#ifndef SILVERBULLET_UTIL_EVENTLOOP_H_
#define SILVERBULLET_UTIL_EVENTLOOP_H_
#include "channel.h"
#include "epoller.h"
#include "util.h"
#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include <iostream>
using namespace std;
namespace SB
{
    namespace util
    {
        class Channel;

        class EventLoop
        {
        public:
            typedef std::function<void()> Task;
            EventLoop();
            ~EventLoop();
            void loop();
            void quit();
            void add_loop_task(Task &&cb);
            void put(Task &&cb);

            void shut_down(shared_ptr<Channel> channel) { shut_down_wr(channel->get_fd()); }

            void remove(shared_ptr<Channel> channel)
            {
                poller_->epoll_del(channel);
            }

            void update(shared_ptr<Channel> channel, int timeout = 0)
            {
                poller_->epoll_mod(channel, timeout);
            }

            void add(shared_ptr<Channel> channel, int timeout = 0)
            {
                poller_->epoll_add(channel, timeout);
            }

        private:
            bool looping_;
            shared_ptr<Epoller> poller_;
            int wakeup_fd_; //唤醒fd
            bool quit_;
            bool eventHandling_;
            mutable std::mutex mutex_;
            std::vector<Task> tasks_;
            bool is_process_task_;
            shared_ptr<Channel> wakeup_channel_;
            void wake_up();
            void handle_read();
            void process_task();
            void handle_env();
        };
    } // namespace util

} // namespace SB

#endif