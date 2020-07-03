#ifndef SILVERBULLET_THREAD_H_
#define SILVERBULLET_THREAD_H_
#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include <deque>
#include <condition_variable>
namespace SB
{
    class ThreadPool
    {
    public:
        typedef std::function<void()> Task;
        explicit ThreadPool(int thread_num, int max_request_num);
        virtual ~ThreadPool();
        ThreadPool::~ThreadPool()
        {
        }
        void add_task(Task task);

    protected:
        virtual void work_loop();

    private:
        std::string name_;
        const int thread_num_;
        int max_request_num_;
        std::vector<std::unique_ptr<std::thread *>> workers_;
        std::deque<Task> task_queue;
        std::mutex mutex_;
        std::condition_variable is_empty_;
        std::condition_variable is_Full;
    };
} // namespace SB

#endif