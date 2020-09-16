#ifndef SILVERBULLET_UTIL_TIMER_H_
#define SILVERBULLET_UTIL_TIMER_H_
#include "httpdata.h"
#include <deque>
#include <memory>
#include <mutex>
#include <set>
#include <unistd.h>
namespace SB
{
    namespace util
    {
        class HttpMessage;
        class TimerNode
        {
        public:
            //  typedef std::chrono::system_clock::time_point TimePoint;
            //  typedef std::chrono::microseconds time;

            TimerNode(std::shared_ptr<HttpMessage> requestData, int timeout);
            ~TimerNode();
            TimerNode(TimerNode &tn);
            void update(int timeout);
            bool isValid();
            void clearReq();
            void setDeleted() { deleted_ = true; }
            bool isDeleted() const { return deleted_; }
            size_t getExpTime() const { return expiredTime_; }

        private:
            bool deleted_;
            size_t expiredTime_;
            std::shared_ptr<HttpMessage> SPHttpData;
        };

        struct TimerCmp
        {
            bool operator()(const std::shared_ptr<TimerNode> &a,
                            const std::shared_ptr<TimerNode> &b) const
            {
                return a->getExpTime() < b->getExpTime();
            }
        };

        /**
         * @brief 定时器管理类
         * 
         */

        class TimerManager
        {
        public:
            TimerManager();
            ~TimerManager();
            void addTimer(std::shared_ptr<HttpMessage> SPHttpData, int timeout);
            void handle_expired();

        private:
            typedef std::shared_ptr<TimerNode> SPTimerNode;
            std::set<SPTimerNode, TimerCmp> timers_;
        };
    } // namespace util

} // namespace SB

#endif