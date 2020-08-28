#ifndef SILVERBULLET_LOG_H_
#define SILVERBULLET_LOG_H_
// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "logstream.h"

class AsyncLogging;

class Logger
{
public:
    Logger(const char *fileName, int line);
    ~Logger();
    LogStream &stream() { return impl_.stream_; }

    static void setLogFileName(std::string fileName) { logFileName_ = fileName; }
    static std::string getLogFileName() { return logFileName_; }

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
    static std::string logFileName_;
};

#define LOG Logger(__FILE__, __LINE__).stream();

class AsyncLog;

class Logger
{
public:
    Logger(const char *log_file_name, int line_num);
    ~Logger();
    LogStream &stream() { return impl_.stream_; }

    static void set_file_name(std::string name) { logFileName_ = name; }
    static std::string getLogFileName() { return logFileName_; }

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
    static std::string file_ = "./WebServer.log";
};
#endif