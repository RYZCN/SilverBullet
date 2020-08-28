#include "threadpool.h"
#include "iostream"
using namespace std;

namespace SB
{
    namespace util
    {
        ThreadPool::ThreadPool(std::string name, int thread_num, int max_requestsize)
            : is_running_(false), name_(name), thread_num_(thread_num), max_task_size_(max_requestsize),
              mutex_(), not_empty_(), not_full_()
        {
        }
        ThreadPool::~ThreadPool()
        {
            stop();
            int nnnn = queue_.size();
        }
        bool ThreadPool::is_running() const
        {
            lock_guard<mutex> lock(mutex_);
            return is_running_;
        }
        int ThreadPool::get_max_size() const
        {
            lock_guard<mutex> lock(mutex_);
            return max_task_size_;
        }
        int ThreadPool::get_thread_num() const
        {
            lock_guard<mutex> lock(mutex_);
            return thread_num_;
        }
        string ThreadPool::get_name() const
        {
            return name_;
        }

        bool ThreadPool::set_max_size(int size)
        {
            bool res = is_running();
            if (res)
                return !res;
            max_task_size_ = size;
            return !res;
        }
        bool ThreadPool::set_thread_num(int num)
        {
            bool res = is_running();
            if (res)
                return !res;
            thread_num_ = num;
            return !res;
        }
        void ThreadPool::start()
        {
            {
                lock_guard<mutex> lock(mutex_);
                is_running_ = true;
            }
            int size = get_thread_num();
            while (size--)
            {
                workers_.emplace_back(new thread(bind(&ThreadPool::work_loop, this)));
            }
        }
        void ThreadPool::put(const Task &task)
        {
            {
                unique_lock<mutex> lock(mutex_);
                while (queue_.size() >= max_task_size_ && is_running_)
                {
                    not_full_.wait(lock);
                }
                if (!is_running_)
                    return;
                queue_.push_back(task);
                not_empty_.notify_one();
            }
        }
        Task ThreadPool::get()
        {
            {
                unique_lock<mutex> lock(mutex_);

                while (queue_.size() <= 0 && is_running_)
                {
                    not_empty_.wait(lock);
                }
                if (queue_.size() > 0)
                {
                    auto res = queue_.front();
                    queue_.pop_front();
                    not_full_.notify_one();
                    return res;
                }
                return Task();
            }
        }
        void ThreadPool::stop()
        {
            {
                lock_guard<mutex> lock(mutex_);
                is_running_ = false;
                not_empty_.notify_all();
                not_full_.notify_all();
            }
            for (auto &thr : workers_)
            {
                thr->join();
            }
        }
        void ThreadPool::work_loop()
        {
            while (is_running())
            {
                auto task = get();
                if (task)
                    task();
            }
        }
    } // namespace util
} // namespace SB
