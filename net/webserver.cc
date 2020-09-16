#include "webserver.h"
#include "logger.h"
#include "util.h"
#include <arpa/inet.h>
#include <functional>
#include <netinet/in.h>
#include <sys/socket.h>
namespace SB
{
    namespace net
    {

        using namespace util;
        Server::Server(EventLoop *loop, int threadNum, int port)
            : loop_(loop),
              threadNum_(threadNum),
              sub_reactor_pool_(new EventLoopThreadPool(loop_, threadNum)),
              started_(false),
              accept_channel_(new Channel(loop_)),
              port_(port),
              listen_fd_(socket_bind_listen(port_))
        {
            accept_channel_->set_fd(listen_fd_);
            handle_for_sigpipe(); //防止往读端关闭写两次使进程关闭
            if (set_socket_nonblock(listen_fd_) < 0)
            {
                perror("set socket non block failed");
                abort();
            }
        }

        void Server::start()
        {
            sub_reactor_pool_->start();

            accept_channel_->set_events(EPOLLIN | EPOLLET);
            accept_channel_->set_read_handler(bind(&Server::handle_new_conn, this));
            accept_channel_->set_env_handler(bind(&Server::handle_env, this));

            loop_->add(accept_channel_, 0);

            started_ = true;
        }

        void Server::handle_new_conn()
        {
            struct sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(struct sockaddr_in));
            socklen_t client_addr_len = sizeof(client_addr);
            int accept_fd = 0;
            while ((accept_fd = accept(listen_fd_, (struct sockaddr *)&client_addr, &client_addr_len)) > 0)
            {
                EventLoop *loop = sub_reactor_pool_->next_loop(); //获取均衡的下一个Reactor
                LOG << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port);

                if (accept_fd >= MAXSIZEOFLINK)
                {
                    close(accept_fd);
                    continue;
                }

                if (set_socket_nonblock(accept_fd) < 0)
                {
                    LOG << "Set non block failed!";
                    return;
                }
                set_socket_no_delay(accept_fd);
                shared_ptr<HttpMessage> req_info(new HttpMessage(loop, accept_fd));
                req_info->channel()->set_holder(req_info);
                loop->put(std::bind(&HttpMessage::add_event, req_info));
            }
            accept_channel_->set_events(EPOLLIN | EPOLLET);
        }
    } // namespace net

} // namespace SB
