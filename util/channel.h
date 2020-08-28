#ifndef SILVERBULLET_CHANNEL_H_
#define SILVERBULLET_CHANNEL_H_
#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <base/noncopyable.h>
#include "eventloop.h"
#include <sys/epoll.h>

class EventLoop;
namespace SB
{
    namespace util
    {
        using CallBack = std::function<void()>;
        /**
         * @brief 维护事件的处理逻辑
         * 
         */
        class Channel
        {
        public:
            Channel(EventLoop *loop, int fd);
            ~Channel();
            void handle();
            int fd() const
            {
                return fd_;
            }
            int event() const
            {
                return event_;
            }
            int revent() const
            {
                return revent_;
            }
            void set_event(const int32_t eve)
            {
                event_ = eve;
            }
            void set_revent(const int32_t eve)
            {
                revent_ = eve;
            }
            void set_read_handle(CallBack cb)
            {
                read_handle_ = std::move(cb);
            }
            void set_write_handle(CallBack cb)
            {
                worite_handle_ = std::move(cb);
            }
            void set_close_handle(CallBack cb)
            {
                close_handle_ = std::move(cb);
            }
            void set_error_handle(CallBack cb)
            {
                error_handle_ = std::move(cb);
            }
            void read();
            void wirte();

        private:
            CallBack read_handle_;
            CallBack worite_handle_;
            CallBack error_handle_;
            CallBack close_handle_;
            EventLoop *loop_;
            int fd_;
            const int fd_;
            uint32_t event_;
            uint32_t revent_;
            bool is_tied_;
            std::weak_ptr<void> owner_; //保活指针，void* 抽象
        };
    } // namespace util
} // namespace SB

#endif