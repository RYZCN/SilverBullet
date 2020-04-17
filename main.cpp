#include "./http/http_conn.h"
#include "./lock/lock.h"
#include "./threadpool/pool.h"

#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <vector>

using namespace std;

extern void addfd(int epollfd, int socketfd);

const int MAX_FD = 65536;
const int MAX_EVENT_NUMBER = 10000;

void destroy_user_list(vector<http_conn *> http_user_list)
{
    for (auto i : http_user_list)
        if (i)
            delete i;
}
int http_conut(vector<http_conn *> http_user_list)
{
    int count = 0;
    for (auto i : http_user_list)
        if (i != nullptr)
            ++count;
    return count;
}
void addfd_lt(int epollfd, int socketfd)
{
    epoll_event event;
    event.data.fd = socketfd;
    event.events = EPOLLIN | EPOLLRDHUP;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &event);
}
/*
主函数中：
1.创建线程池
2.根据最大描述符来创建http请求数组，用来根据不同请求的不同描述符来记录各个http请求
3.socket,bind,listen,epool_wait,得到io事件event
4.对于新的连接请求，接受，保存入http请求数组进行初始化
5.对于读写请求，请求入队
*/

int main(int argc, char *agrv[])
{
    int port;
    if (argc <= 1)
    {
        cout << "open default port : 12345" << endl;
        port = 12345;
    }
    else
    {
        port = atoi(agrv[1]);
    }

    pool<http_conn> m_threadpool(10, 10000);             //创建线程池对象
    vector<http_conn *> http_user_list(MAX_FD, nullptr); //连接数量取决于描述符的数量,这个太奇怪了，析构什么的
    int user_count = 0;                                  //用户数为0

    int listenfd = socket(PF_INET, SOCK_STREAM, 0); //socket
    struct sockaddr_in address;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    int flag = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    bind(listenfd, (struct sockaddr *)&address, sizeof(address));

    listen(listenfd, 5);
    //cout << "listen, listenfd : " << listenfd << endl;

    //创建内核事件表
    epoll_event event_list[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    addfd_lt(epollfd, listenfd);
    http_conn::m_epollfd = epollfd; //更新http_conn类中的epoll描述符

    while (1)
    {
        int number = epoll_wait(epollfd, event_list, MAX_EVENT_NUMBER, -1);
        for (int i = 0; i < number; ++i)
        {
            int socketfd = event_list[i].data.fd;
            if (socketfd == listenfd)
            {
                //cout << "new client" << endl;
                //新的连接
                struct sockaddr_in client_addr;
                socklen_t client_addr_length = sizeof(client_addr);
                int connfd = accept(listenfd, (struct sockaddr *)(&client_addr), &client_addr_length);
                if (connfd < 0)
                {
                    printf("%s\n", strerror(errno));
                    continue; //跳过这一轮
                }
                if (http_conn::m_user_count >= MAX_FD)
                    continue;
                if (http_user_list[connfd] == nullptr)
                {
                    cout << "new one http  " << connfd << endl;
                    http_user_list[connfd] = new http_conn();
                }
                http_user_list[connfd]->init(connfd, client_addr);
            }
            else if (event_list[i].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
            {
                //cout << "error!!" << endl;
                //printf("%s\n", strerror(errno));
                http_user_list[socketfd]->close_conn("error!");
            }
            else if (event_list[i].events & EPOLLIN)
            {
                //cout << "old client" << socketfd << endl;
                //客户的数据要读进来
                if (http_user_list[socketfd]->read())
                {
                    //cout << "we read the request from brower" << endl;
                    //如果读到了一些数据（已经读到了读缓存区里）,就把这个请求放到工作线程的请求队列中
                    m_threadpool.append(http_user_list[socketfd]);
                }
                else
                {
                    cout << "read error!!!!!!!!!!" << endl;
                    //没有读到数据的话
                    http_user_list[socketfd]->close_conn("read nothing");
                }
            }
            else if (event_list[i].events & EPOLLOUT)
            {
                //cout << "want to write" << endl;
                http_user_list[socketfd]->write();
                http_user_list[socketfd]->close_conn("write over");
            }
        }
    }
    close(epollfd);
    close(listenfd);
    destroy_user_list(http_user_list);
    //cout << "4 ok" << endl;

    return 0;
}