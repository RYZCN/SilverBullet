#include "threadpool.h"
using namespace std;

namespace SB
{
    ThreadPool::ThreadPool(std::string name, int thread_num, int max_request_num)
        : name_(name),
          thread_num_(thread_num),
          max_request_num_(max_request_num),
          mutex_(),
          not_empty_(),
          not_full_(),
          running_(true)
    {
        for (size_t i = 0; i < thread_num_; i++)
        {
            workers_.emplace_back(new thread(bind(&ThreadPool::work_loop, this)));
        }
    }

    ThreadPool::~ThreadPool()
    {
        {
            unique_lock<mutex> lock(mutex_);
            running_ = false;
            not_empty_.notify_all();
            not_full_.notify_all();
        }
        for (auto &ele : workers_)
        {
            ele->join();
        }
    }
    void ThreadPool::work_loop()
    {
        while (running_)
        {
            Task runItem = take();
            if (runItem)
            {
                runItem();
            }
        }
    }

    void ThreadPool::run(Task task)
    {
        unique_lock<mutex> lock(mutex_);
        while (task_queue_.size() >= max_request_num_ && running_)
        {
            not_full_.wait(lock);
        }
        task_queue_.push_back(task);
        not_empty_.notify_one();
    }
    ThreadPool::Task ThreadPool::take()
    {
        Task res;
        {
            unique_lock<mutex> lock(mutex_);

            while (task_queue_.empty() && running_)
            {
                not_empty_.wait(lock);
            }

            res = task_queue_.front();
            task_queue_.pop_front();
            not_full_.notify_one();
        }

        return res;
    }
} // namespace SB
