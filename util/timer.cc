#include "timer.h"

#include "httpdata.h"
#include <chrono>
#include <queue>
#include <sys/time.h>
#include <unistd.h>
namespace SB
{
  namespace util
  {
    TimerNode::TimerNode(std::shared_ptr<HttpMessage> requestData, int timeout)
        : deleted_(false), SPHttpData(requestData)
    {
      struct timeval now;
      gettimeofday(&now, NULL);
      expiredTime_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
    }

    TimerNode::~TimerNode()
    {
      if (SPHttpData)
        SPHttpData->handle_close();
    }

    TimerNode::TimerNode(TimerNode &tn)
        : SPHttpData(tn.SPHttpData), expiredTime_(0) {}

    void TimerNode::update(int timeout)
    {
      struct timeval now;
      gettimeofday(&now, NULL);
      expiredTime_ =
          (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
    }

    bool TimerNode::isValid()
    {
      struct timeval now;
      gettimeofday(&now, NULL);
      size_t temp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
      if (temp < expiredTime_)
        return true;
      else
      {
        this->setDeleted();
        return false;
      }
    }

    void TimerNode::clearReq()
    {
      SPHttpData.reset();
      this->setDeleted();
    }

    TimerManager::TimerManager() {}

    TimerManager::~TimerManager() {}

    void TimerManager::addTimer(std::shared_ptr<HttpMessage> SPHttpData, int timeout)
    {
      SPTimerNode new_node(new TimerNode(SPHttpData, timeout));
      timers_.insert(new_node);
      SPHttpData->linkTimer(new_node);
    }

    void TimerManager::handle_expired()
    {
      auto it = timers_.begin();
      for (; it != timers_.end(); ++it)
      {
        if ((*it)->isValid())
          break;
      }
      timers_.erase(timers_.begin(), it);
    }
  } // namespace util

} // namespace SB
