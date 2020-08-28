#include "epoller.h"
#include <unistd.h>
using namespace std;
namespace SB
{
    namespace util
    {
        Epoller::Epoller(EventLoop *owner)
            : owner_(owner)
        {
            fd_ = epoll_create1(EPOLL_CLOEXEC);
            if (fd_ < 0)
            {
                //TODO:
            }
        }
        Epoller::~Epoller()
        {
            ::close(fd_);
        }

    } // namespace util

} // namespace SB
