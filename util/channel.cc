#include "channel.h"
#include "epoller.h"
#include "eventloop.h"
#include "util.h"
#include <cstdlib>
#include <iostream>
#include <queue>
#include <unistd.h>
namespace SB
{
    namespace util
    {

        using namespace std;

        Channel::Channel(EventLoop *loop)
            : loop_(loop), events_(0), lastEvents_(0), fd_(0) {}

        Channel::Channel(EventLoop *loop, int fd)
            : loop_(loop), fd_(fd), events_(0), lastEvents_(0) {}

        Channel::~Channel()
        {
        }

        int Channel::get_fd() { return fd_; }
        void Channel::set_fd(int fd) { fd_ = fd; }

        void Channel::handle_read()
        {
            if (readHandler_)
            {
                readHandler_();
            }
        }

        void Channel::handle_wirte()
        {
            if (writeHandler_)
            {
                writeHandler_();
            }
        }
        void Channel::handle_event_process_env()
        {
            if (connHandler_)
            {
                connHandler_();
            }
        }
    } // namespace util

} // namespace SB
