#ifndef SILVERBULLET_BASE_SINGLETON_H_
#define SILVERBULLET_BASE_SINGLETON_H_
#include <atomic>
#include <mutex>

#include "noncopyable.h"

template <typename T>
class Singleton : noncopyable
{
public:
    Singleton() = delete;
    ~Singleton()
    {
        delete instance_;
        instance_ = nullptr;
    }
    static T &get_instance()
    {
        Singleton *tmp = instance_.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire); //获取内存fence
        if (tmp == nullptr)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tmp = instance_.load(std::memory_order_relaxed);
            if (tmp == nullptr)
            {
                instance_ = new T();
                std::atomic_thread_fence(std::memory_order_release); //释放内存fence
                instance_.store(tmp, std::memory_order_relaxed);
            }
        }
        return tmp;
    }

private:
    static std::mutex mutex_;
    static std::atomic<T *> instance_;
};
template <typename T>
std::mutex Singleton<T>::mutex_ = std::mutex();

template <typename T>
std::atomic<T *> Singleton<T>::instance_ = nullptr;

#endif