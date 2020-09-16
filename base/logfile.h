#ifndef SILVERBULLET_BASE_LOGFILE_H_
#define SILVERBULLET_BASE_LOGFILE_H_
#include "noncopyable.h"
#include <memory>
#include <mutex>
#include <string>

class AppendFile;
class LogFile : noncopyable
{
public:
    LogFile(const std::string &name, int flush_every = 1024);
    ~LogFile();

    void append(const char *line, int len);
    void flush();
    bool roll();

private:
    void append_unlocked(const char *line, int len);

    const std::string name_;
    const int flush_every_;
    int count_;
    std::mutex mutex_;
    std::unique_ptr<AppendFile> file_;
};
#endif