
#include "logstream.h"
#include <algorithm>
#include <assert.h>
#include <limits>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const char digits[] = "9876543210123456789";
const char *zero = digits + 9;

// 木铎的实现
template <typename T>
size_t convert(char buf[], T value)
{
    T i = value;
    char *p = buf;

    do
    {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0)
    {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

template class FixedBuffer<SBUFFSIZE>;
template class FixedBuffer<LBUFFSIZE>;

template <typename T>
void LogStream::formatInteger(T v)
{
    // buffer容不下最大字符串的话会被直接丢弃
    if (buffer_.rest() >= MAX_NUM_LENGTH)
    {
        size_t len = convert(buffer_.current(), v);
        buffer_.add(len);
    }
}

LogStream &LogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;
}

LogStream &LogStream::operator<<(int v)
{
    formatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(long long v)
{
    formatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(double v)
{
    if (buffer_.rest() >= MAX_NUM_LENGTH)
    {
        int len = snprintf(buffer_.current(), MAX_NUM_LENGTH, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}

LogStream &LogStream::operator<<(long double v)
{
    if (buffer_.rest() >= MAX_NUM_LENGTH)
    {
        int len = snprintf(buffer_.current(), MAX_NUM_LENGTH, "%.12Lg", v);
        buffer_.add(len);
    }
    return *this;
}