#include "channel.h"
namespace SB
{
    namespace util
    {
        Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd), events_()
        {
        }
        void Channel::handle()
        {
        }
    } // namespace util

} // namespace SB
