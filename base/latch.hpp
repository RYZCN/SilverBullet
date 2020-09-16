#ifndef SILVERBULLET_BASE_LATCH_HPP_
#define SILVERBULLET_BASE_LATCH_HPP_

#include "noncopyable.h"
#include <condition_variable>
#include <mutex>

class Latch : noncopyable
{
public:
    explicit Latch(int count) : mutex_(), count_(count) {}
    /**
     * @brief 主线程阻塞在此处，直到被条件变量唤醒
     * 
     */
    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (count_ > 0)
            zero_cond_.wait(lock);
    }
    /**
     * @brief 计数器减少至0时唤醒条件变量
     * 
     */
    void count_down()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        --count_;
        if (count_ == 0)
            zero_cond_.notify_all();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable zero_cond_;
    int count_; //计数器
};
#endif