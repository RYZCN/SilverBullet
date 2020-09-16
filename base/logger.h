#ifndef SILVERBULLET_BASE_LOG_H_
#define SILVERBULLET_BASE_LOG_H_

#include "logstream.h"
#include <bits/stdc++.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <string>
class AsyncLog;

class Logger
{
public:
    Logger(const char *log_file_name, int line_num);
    ~Logger();
    LogStream &stream() { return impl_.stream_; }
    static void set_file_name(std::string name) { file_ = name; }
    static std::string get_log_file_name() { return file_; }

private:
    class Impl
    {
    public:
        Impl(const char *fileName, int line);
        void formatTime();

        LogStream stream_;
        int line_;
        std::string basename_;
    };
    Impl impl_;
    static std::string file_;
};
#define LOG Logger(__FILE__, __LINE__).stream()

#endif