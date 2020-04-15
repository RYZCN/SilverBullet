#include "http_conn.h"

void setnonblocking(int socketfd)
{
    int old_opt = fcntl(socketfd, F_GETFL);
    int new_opt = old_opt | O_NONBLOCK;
    fcntl(socketfd, F_SETFL, new_opt);
}

void addfd(int epollfd, int socketfd)
{
    //把该描述符添加到epoll的事件表
    epoll_event m_event;                                   //新建一个事件
    m_event.data.fd = socketfd;                            //使得描述符为本描述符
    m_event.events = EPOLLIN | EPOLLET | EPOLLHUP;         //监听输入、边缘触发、挂起？ 知识点，什么叫挂起？
    epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &m_event); //将该事件添加，知识点，ctl的作用？
    setnonblocking(socketfd);                              //设置非阻塞，什么叫阻塞非阻塞
}

void removefd(int epollfd, int socketfd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, socketfd, 0); //把该套接字的对应事件删除
    close(socketfd);                                //关闭该套接字
}

void modfd(int epollfd, int socketed, int ev)
{
    epoll_event m_event;
    m_event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLHUP;
    m_event.data.fd = socketed;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, socketed, &m_event);
}

int http_conn::m_user_count = 0;
int http_conn::m_epollfd = -1;

void http_conn::init(int socketfd, const sockaddr_in &addr) //套接字的初始化，用于添加进epoll的事件表
{
    m_socket = socketfd;
    m_addr = addr;
    addfd(m_epollfd, m_socket);

    memset(read_buff, '\0', BUFF_READ_SIZE); //清空缓冲区
    memset(write_buff, '\0', BUFF_WRITE_SIZE);
    read_for_now = 0;
    write_for_now = 0;
}

void http_conn::close_conn()
{
    //将当前的描述符在epoll监听事件里面去除
    if (m_socket != -1)
    {
        removefd(m_epollfd, m_socket);
        m_user_count--;
        cout << "we closed socket:" << m_socket << endl;
        m_socket = -1; //-1就是代表没有正在连接的套接字
    }
}

void http_conn::process() //对请求进行处理
{
    //cout << "begin to process the request" << endl;
    //首先进行报文的解析
    HTTP_CODE ret = process_read();
    //然后进行报文的响应
    bool result = process_write(ret);
    //最后向epoll的监听的事件表中添加可写事件
    modfd(m_epollfd, m_socket, EPOLLOUT);
    //cout << "add writeble event to apoll_list" << endl;
}
http_conn::HTTP_CODE http_conn::process_read()
{
    //从读缓冲区读取，并且进行解析,主状态机预警

    //首先初始化主、从状态机的状态
    HTTP_CODE ret = NO_REQUEST;
    LINE_STATE line_ret = LINE_OK;
    string text; // 初始化字符串，用于读取数据

    //先不考虑解析http协议,统一认为这个是get请求
    return GET_REQUEST;
}

bool http_conn::process_write(HTTP_CODE ret)
{
    //先随便返回什么东西

    string temp = "HTTP/1.1 200 OK\r\n\r\n<html><b>hello world!</b></html>";
    if (temp.size() + 1 > BUFF_WRITE_SIZE)
    {
        cout << "too long" << endl;
    }
    else
    {
        strcpy(write_buff, temp.c_str());
    }

    return true;
}
bool http_conn::read() //把socket的东西全部读到读缓冲区里面
{
    if (read_for_now > BUFF_READ_SIZE) //如果当前可以写入读缓冲区的位置已经超出了缓冲区长度了
    {
        return false;
    }
    int bytes_read = 0;
    //return true;
    while (true)
    {
        bytes_read = recv(m_socket, read_buff + read_for_now, BUFF_READ_SIZE - read_for_now, 0);
        if (bytes_read == -1) //出现错误
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) //如果是因为。。。和，，，原因导致的话，就顺利退出循环
            {
                break;
            }
            cout << "bytes_read == -1" << endl;
            return false;
        }
        else if (bytes_read == 0) //没东西可读？
        {
            cout << "bytes_read == 0" << endl;
            return false; //读完了为什么返回0？
            continue;
        }
        //cout << "we read::" << read_buff << endl;
        read_for_now += bytes_read;
    }
    return true;
}

bool http_conn::write() //把响应的内容写到写缓冲区中
{
    int bytes_write = 0;
    //先不考虑大文件的情况
    bytes_write = send(m_socket, write_buff, BUFF_WRITE_SIZE, 0);

    if (bytes_write <= 0)
    {
        return false;
    }
    modfd(m_epollfd, m_socket, EPOLLIN);

    //发送完后在事件表移除这个描述符?真的假的？
    //removefd(m_epollfd, m_socket);
    return true;
}
