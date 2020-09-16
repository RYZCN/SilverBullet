// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "util.h"

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const int MAX_BUFF = 4096;
ssize_t readn(int fd, void *buff, size_t n)
{
    size_t left_num = n;
    ssize_t read_num = 0;
    ssize_t sum = 0;

    char *ptr = (char *)buff;
    while (left_num > 0)
    {
        if ((read_num = read(fd, ptr, left_num)) < 0)
        {
            if (errno == EINTR) //中断重试
                read_num = 0;
            else if (errno == EAGAIN) //阻塞情况下无就绪数据
            {
                return sum;
            }
            else
            {
                return -1;
            }
        }
        else if (read_num == 0)
            break;
        sum += read_num;
        left_num -= read_num;
        ptr += read_num;
    }
    return sum;
}

ssize_t readn(int fd, std::string &inBuffer, bool &zero)
{
    ssize_t read_num = 0;
    ssize_t sum = 0;
    while (true)
    {
        char buff[MAX_BUFF];
        if ((read_num = read(fd, buff, MAX_BUFF)) < 0)
        {
            if (errno == EINTR)
                continue;
            else if (errno == EAGAIN)
            {
                return sum;
            }
            else
            {
                perror("read error");
                return -1;
            }
        }
        else if (read_num == 0)
        {
            zero = true;
            break;
        }
        sum += read_num;
        inBuffer += std::string(buff, buff + read_num);
    }
    return sum;
}

ssize_t readn(int fd, std::string &inBuffer)
{
    ssize_t read_num = 0;
    ssize_t sum = 0;
    while (true)
    {
        char buff[MAX_BUFF];
        if ((read_num = read(fd, buff, MAX_BUFF)) < 0)
        {
            if (errno == EINTR)
                continue;
            else if (errno == EAGAIN)
            {
                return sum;
            }
            else
            {
                perror("read error");
                return -1;
            }
        }
        else if (read_num == 0)
        {

            break;
        }

        sum += read_num;
        inBuffer += std::string(buff, buff + read_num);
    }
    return sum;
}

ssize_t writen(int fd, void *buff, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    char *ptr = (char *)buff;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0)
            {
                if (errno == EINTR)
                {
                    nwritten = 0;
                    continue;
                }
                else if (errno == EAGAIN)
                {
                    return writeSum;
                }
                else
                    return -1;
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeSum;
}

ssize_t writen(int fd, std::string &sbuff)
{
    size_t left_num = sbuff.size();
    ssize_t write_num = 0;
    ssize_t sum = 0;
    const char *ptr = sbuff.c_str();
    while (left_num > 0)
    {
        if ((write_num = write(fd, ptr, left_num)) <= 0)
        {
            if (write_num < 0)
            {
                if (errno == EINTR)
                {
                    write_num = 0;
                    continue;
                }
                else if (errno == EAGAIN)
                    break;
                else
                    return -1;
            }
        }
        sum += write_num;
        left_num -= write_num;
        ptr += write_num;
    }
    if (sum == static_cast<int>(sbuff.size()))
        sbuff.clear();
    else
        sbuff = sbuff.substr(sum);
    return sum;
}

void handle_for_sigpipe()
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa, NULL)) //不退进程
        return;
}

int set_socket_nonblock(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1)
        return -1;

    flag |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flag) == -1)
        return -1;
    return 0;
}

void set_socket_no_delay(int fd)
{
    int enable = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable)); //允许小包发送
}

void setSocketNoLinger(int fd)
{
    struct linger linger_;
    linger_.l_onoff = 1;
    linger_.l_linger = 30;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *)&linger_, sizeof(linger_));
}

void shut_down_wr(int fd)
{
    shutdown(fd, SHUT_WR);
}

int socket_bind_listen(int port)
{
    // 检查port值，取正确区间范围
    if (port < 0 || port > 65535)
        return -1;

    // 创建socket(IPv4 + TCP)，返回监听描述符
    int listen_fd = 0;
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) //
        return -1;

    // 消除bind时"Address already in use"错误
    int optval = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
        close(listen_fd);
        return -1;
    }

    // 设置服务器IP和Port，和监听描述副绑定
    struct sockaddr_in server_addr;
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short)port);
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        close(listen_fd);
        return -1;
    }

    // 开始监听，最大等待队列长为LISTENQ
    if (listen(listen_fd, 2048) == -1)
    {
        close(listen_fd);
        return -1;
    }

    // 无效监听描述符
    if (listen_fd == -1)
    {
        close(listen_fd);
        return -1;
    }
    return listen_fd;
}