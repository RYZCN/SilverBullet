#ifndef SILVERBULLET_BASE_ASYNCLOG_H_
#define SILVERBULLET_BASE_ASYNCLOG_H_
#include "latch.hpp"
#include "logstream.h"
#include "noncopyable.h"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using Buffer = FixedBuffer<LBUFFSIZE>;
using BufferPtr = std::shared_ptr<Buffer>;
using BufferVector = std::vector<BufferPtr>;

/**
 * @brief 
 * 
 */
class AsyncLog : noncopyable
{
public:
    typedef std::chrono::seconds Seconds;
    AsyncLog(const std::string basename, int flushInterval = 2);
    ~AsyncLog();
    void append(const char *logline, int len);

    void start();

    void stop();

private:
    void back_up_func();

    const int flush_interval_;
    bool is_running_;
    std::string name_;

    std::thread back_thread_;
    std::mutex mutex_;
    /**
     * @brief 条件变量唤醒条件：有buffer要写：一、前端写满 二、时间超过两秒
     * 
     */
    std::condition_variable buf_to_write_cond_;

    BufferPtr curr_buf_;
    BufferPtr next_buf_;
    BufferVector buffers_;
    Latch latch_;
};
#endif