#ifndef SILVERBULLET_NET_WEBSERVER_H_
#define SILVERBULLET_NET_WEBSERVER_H_
#include "channel.h"
#include "eventloop.h"
#include "eventloopthreadpool.h"
#include <memory>
namespace SB
{
    namespace net
    {
        using namespace util;
        class Server
        {
        public:
            Server(EventLoop *loop, int threadNum, int port);
            ~Server() {}
            EventLoop *loop_ptr() const { return loop_; }
            void start();
            void handle_new_conn();
            void handle_env() { loop_->update(accept_channel_); }

        private:
            EventLoop *loop_;
            int threadNum_;
            std::unique_ptr<EventLoopThreadPool> sub_reactor_pool_;
            bool started_;
            std::shared_ptr<Channel> accept_channel_;
            int port_;
            int listen_fd_;
            static const int MAXSIZEOFLINK = 100000; //最大链接数
        };
    } // namespace net

} // namespace SB

#endif