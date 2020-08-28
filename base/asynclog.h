#ifndef SILVERBULLET_ASYNCLOG_H_
#define SILVERBULLET_ASYNCLOG_H_
#include <functional>
#include <string>
#include <vector>
#include <mutex>
#include <thread>

#include "coutdownlatch.h"
#include "logstream.h"
#include "noncopyable.h"


using Buffer = FixedBuffer<kLargeBuffer>;
using BufferPtr = std::shared_ptr<Buffer>;
using BufferVector = std::vector<BufferPtr>;


class AsyncLogging : noncopyable
{
public:
    AsyncLogging(const std::string basename, int flushInterval = 2);
    ~AsyncLogging();
    void append(const char *logline, int len);

    void start();

    void stop();

private:
    void threadFunc();

    const int flushInterval_;
    bool is_running_;
    std::string name_;

    std::thread thread_;

    std::mutex mutex_;
    std::condition_variable cond_;

    BufferPtr curr_buf_;
    BufferPtr next_buf_;
    BufferVector buffers_;

    CountDownLatch latch_;
};
#endif