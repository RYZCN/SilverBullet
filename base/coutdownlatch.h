#ifndef SILVERBULLET_COUNTDOWNLATCH_H_
#define SILVERBULLET_COUNTDOWNLATCH_H_

#include <mutex>
#include <condition_variable>
#include "noncopyable.h"

class CountDownLatch : noncopyable
{
public:
    explicit CountDownLatch(int count) : mutex_(), count_(count) {}
    /**
     * @brief 主线程阻塞在此处，直到被条件变量唤醒
     * 
     */
    void wait()
    {
        std::unique_lock lock(mutex_);
        while (count_ > 0)
            zero_cond_.wait(lock);
    }
    /**
     * @brief 计数器减少至0时唤醒条件变量
     * 
     */
    void count_down()
    {
        std::lock_guard lock(mutex_);
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