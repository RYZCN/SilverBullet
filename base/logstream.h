#ifndef SILVERBULLET_LOGSTREAM_H_
#define SILVERBULLET_LOGSTREAM_H_
#include <assert.h>
#include <string.h>
#include <string>
#include "noncopyable.h"

class AsyncLogging;
const int SBUFFSIZE = 4096;
const int LBUFFSIZE = 4096 * 1000;

template <int SIZE>
class FixedBuffer : noncopyable
{
public:
    FixedBuffer() : cur_(data_) {}

    ~FixedBuffer() {}

    void append(const char *buf, size_t len)
    {
        if (rest() > static_cast<int>(len))
        {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    const char *data() const { return data_; }
    int length() const { return static_cast<int>(cur_ - data_); }

    char *current() { return cur_; }
    int rest() const { return static_cast<int>(end() - cur_); }
    void add(size_t len) { cur_ += len; }

    void reset() { cur_ = data_; }
    void init_zero() { memset(data_, 0, sizeof data_); }

private:
    const char *end() const { return data_ + sizeof data_; }

    char data_[SIZE];
    char *cur_;
};

class Stream : noncopyable
{
public:
    typedef FixedBuffer<SBUFFSIZE> Buffer;

    Stream &operator<<(bool v)
    {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
    }

    Stream &operator<<(short);
    Stream &operator<<(unsigned short);
    Stream &operator<<(int);
    Stream &operator<<(unsigned int);
    Stream &operator<<(long);
    Stream &operator<<(unsigned long);
    Stream &operator<<(long long);
    Stream &operator<<(unsigned long long);

    Stream &operator<<(const void *);

    Stream &operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }
    Stream &operator<<(double);
    Stream &operator<<(long double);

    Stream &operator<<(char v)
    {
        buffer_.append(&v, 1);
        return *this;
    }

    Stream &operator<<(const char *str)
    {
        if (str)
            buffer_.append(str, strlen(str));
        else
            buffer_.append("(null)", 6);
        return *this;
    }

    Stream &operator<<(const unsigned char *str)
    {
        return operator<<(reinterpret_cast<const char *>(str));
    }

    Stream &operator<<(const std::string &v)
    {
        buffer_.append(v.c_str(), v.size());
        return *this;
    }

    void append(const char *data, int len) { buffer_.append(data, len); }
    const Buffer &buffer() const { return buffer_; }
    void reset_buf() { buffer_.reset(); }

private:
    void staticCheck();

    template <typename T>
    void formatInteger(T);

    Buffer buffer_;

    static const int MAX_NUM_LENGTH = 32;
};

#endif