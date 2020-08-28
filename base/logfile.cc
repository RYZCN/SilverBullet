#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "logfile.h"
class AppendFile : noncopyable
{
public:
    explicit AppendFile(std::string filename) : fp_(fopen(filename.c_str(), "ae"))
    {
        setbuffer(fp_, buffer_, sizeof buffer_);
    }
    ~AppendFile() { fclose(fp_); }
    void append(const char *line, const size_t len)
    {
        size_t n = this->write(line, len);
        size_t remain = len - n;
        while (remain > 0)
        {
            size_t x = this->write(line + n, remain);
            if (x == 0)
            {
                int err = ferror(fp_);
                if (err)
                    fprintf(stderr, "AppendFile::append() failed !\n");
                break;
            }
            n += x;
            remain = len - n;
        }
    }
    void flush() { fflush(fp_); }

private:
    size_t write(const char *line, size_t len)
    {
        return fwrite_unlocked(line, 1, len, fp_); //后端单线程
    }
    FILE *fp_;
    char buffer_[0xffff + 1];
};

using namespace std;
LogFile::LogFile(const string &name, int flushEveryN)
    : name_(name),
      flush_every_(flushEveryN),
      count_(0),
      mutex_()
{
    file_.reset(new AppendFile(name));
}

LogFile::~LogFile() {}

void LogFile::append(const char *logline, int len)
{
    lock_guard lock(mutex_);
    append_unlocked(logline, len);
}

void LogFile::flush()
{
    lock_guard lock(mutex_);
    file_->flush();
}
void LogFile::append_unlocked(const char *line, int len)
{
    file_->append(line, len);
    ++count_;
    if (count_ >= flush_every_)
    {
        count_ = 0;
        file_->flush();
    }
}