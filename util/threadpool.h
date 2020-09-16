#ifndef SILVERBULLET_UTIL_THREAD_H_
#define SILVERBULLET_UTIL_THREAD_H_
#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
namespace SB
{
    namespace util
    {

        const int DEFAULT_THREAD_NUM = 4;
        const int DEFAULT_MAX_TASK_SIZE = 0xFFFF;
        using Task = std::function<void()>;
        class ThreadPool
        {
        public:
            explicit ThreadPool(std::string name, int thread_num = DEFAULT_THREAD_NUM, int max_requestsize = DEFAULT_MAX_TASK_SIZE);
            ~ThreadPool();

            bool is_running() const;
            std::string get_name() const;
            int get_max_size() const;
            int get_thread_num() const;

            bool set_max_size(int size);
            bool set_thread_num(int num);

            void start();
            void put(const Task &task);
            Task get();

            void stop();

        protected:
            void work_loop();

        private:
            bool is_running_ = false;
            int max_task_size_;
            int thread_num_;
            mutable std::mutex mutex_;
            std::condition_variable not_empty_;
            std::condition_variable not_full_;
            std::vector<std::unique_ptr<std::thread>> workers_;
            std::deque<Task> queue_;
            std::string name_;
        };
    } // namespace util

} // namespace SB

#endif