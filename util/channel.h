#ifndef SILVERBULLET_UTIL_CHANNEL_H_
#define SILVERBULLET_UTIL_CHANNEL_H_

#include "noncopyable.h"
#include "timer.h"
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <sys/epoll.h>
#include <thread>
#include <unordered_map>
#include <vector>

namespace SB
{
    namespace util
    {

        class EventLoop;
        class HttpMessage;

        class Channel
        {
        private:
            typedef std::function<void()> CallBack;
            EventLoop *loop_;
            int fd_;
            __uint32_t events_;
            __uint32_t revents_;
            __uint32_t lastEvents_;

            //owner
            std::weak_ptr<HttpMessage> holder_;

        private:
            int parse_URI();
            int parse_Headers();
            int analys_request();

            CallBack readHandler_;
            CallBack writeHandler_;
            CallBack errorHandler_;
            CallBack connHandler_;

        public:
            Channel(EventLoop *loop);
            Channel(EventLoop *loop, int fd);
            ~Channel();
            int get_fd();
            void set_fd(int fd);

            void set_holder(std::shared_ptr<HttpMessage> holder) { holder_ = holder; }
            std::shared_ptr<HttpMessage> get_holder()
            {
                std::shared_ptr<HttpMessage> ret(holder_.lock());
                return ret;
            }

            void set_read_handler(CallBack &&readHandler)
            {
                readHandler_ = readHandler;
            }
            void set_write_handler(CallBack &&writeHandler)
            {
                writeHandler_ = writeHandler;
            }
            void set_err_handler(CallBack &&errorHandler)
            {
                errorHandler_ = errorHandler;
            }
            void set_env_handler(CallBack &&connHandler)
            {
                connHandler_ = connHandler;
            }

            void handle_events()
            {
                events_ = 0;
                if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
                {
                    events_ = 0;
                    return;
                }
                if (revents_ & EPOLLERR)
                {
                    if (errorHandler_)
                        errorHandler_();
                    events_ = 0;
                    return;
                }
                if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
                {
                    handle_read();
                }
                if (revents_ & EPOLLOUT)
                {
                    handle_wirte();
                }
                handle_event_process_env();
            }
            void handle_read();
            void handle_wirte();
            void handle_error(int fd, int err_num, std::string short_msg);
            void handle_event_process_env();

            void set_revents(__uint32_t ev) { revents_ = ev; }

            void set_events(__uint32_t ev) { events_ = ev; }
            __uint32_t &get_events() { return events_; }

            bool update_event_and_judge_is_same()
            {
                bool ret = (lastEvents_ == events_);
                lastEvents_ = events_;
                return ret;
            }

            __uint32_t get_last_events() { return lastEvents_; }
        };

        typedef std::shared_ptr<Channel> SP_Channel;
    } // namespace util
} // namespace SB

#endif