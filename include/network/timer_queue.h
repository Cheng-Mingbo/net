//
// Created by Cheng MingBo on 2023/5/13.
//

#ifndef LIBNET_TIMER_QUEUE_H
#define LIBNET_TIMER_QUEUE_H
#include <set>
#include <vector>
#include <mutex>
#include <atomic>
#include <memory>
#include "Timestamp.h"
#include "channel.h"

class EventLoop;
class Timer;
class TimerId;

class TimerQueue {
  public:
    using TimerCallback = std::function<void()>;
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();
    
    TimerId addTimer(TimerCallback cb, Timestamp when, double interval);
    void cancel(const TimerId& timerId);
  
  private:
    using Entry = std::pair<Timestamp, std::shared_ptr<Timer>>; // <到期时间，定时器>
    using TimerList = std::set<Entry>; // 定时器列表
    using ActiveTimer = std::pair<std::shared_ptr<Timer>, int64_t>; // <定时器，序列号>
    using ActiveTimerSet = std::set<ActiveTimer>; // 活跃定时器集合
    
    void addTimerInLoop(const std::shared_ptr<Timer>& timer); // 在IO线程中添加定时器
    void cancelInLoop(const TimerId& timerId); // 在IO线程中取消定时器
    void handleRead(); // 定时器到期后的回调函数
    void getExpired(Timestamp now, std::vector<TimerQueue::Entry>&); // 获取到期的定时器
    void reset(const std::vector<Entry>& expired, Timestamp now); // 重置定时器
    bool insert(const std::shared_ptr<Timer>& timer); // 插入定时器
    
    EventLoop* loop_; // 所属IO线程
    const int timerfd_; // 定时器文件描述符
    Channel timerfdChannel_; // 定时器事件通道
    TimerList timers_; // 定时器列表
    ActiveTimerSet activeTimers_; // 活跃定时器集合
    std::atomic_bool callingExpiredTimers_; // 是否正在处理到期的定时器
    ActiveTimerSet cancelingTimers_; // 取消的定时器集合
};

#endif //LIBNET_TIMER_QUEUE_H
