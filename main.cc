#include "eventloop.h"
#include "logger.h"
#include "webserver.h"
#include <getopt.h>
#include <string>
using namespace SB::util;
using namespace SB::net;
int main(int argc, char *argv[])
{
    int thread_num = 4;
    int port = 9999;
    std::string logPath = "./WebServer.log";
    int opt;
    const char *str = "t:l:p:";
    while ((opt = getopt(argc, argv, str)) != -1)
    {
        switch (opt)
        {
        case 't':
        {
            thread_num = atoi(optarg);
            break;
        }
        case 'l':
        {
            logPath = optarg;
            if (logPath.size() < 2 || optarg[0] != '/')
            {
                printf("logPath should start with \"/\"\n");
                abort();
            }
            break;
        }
        case 'p':
        {
            port = atoi(optarg);
            break;
        }
        default:
            break;
        }
    }
    Logger::set_file_name(logPath);
// STL库在多线程上应用
#ifndef _PTHREADS
    LOG << "_PTHREADS is not defined !";
#endif
    EventLoop accept_loop;
    Server web_server(&accept_loop, thread_num, port);
    web_server.start();
    accept_loop.loop();
    return 0;
}
