//
// Created by Cheng MingBo on 2023/5/13.
//
#include <sys/timerfd.h>
#include <cstring>
#include "timer_queue.h"
#include "logger.h"
#include "event_loop.h"
#include "timer.h"
#include "timer_id.h"

int createTimerfd() {
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0) {
        logger->error("Failed in timerfd_create");
    }
    return timerfd;
}

// 计算超时时刻与当前时间的时间差
struct timespec howMuchTimeFromNow(Timestamp when) {
    int64_t microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
    if (microseconds < 100) {
        microseconds = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}


void readTimerfd(int timerfd, Timestamp now) { // 读取定时器的超时次数，避免一直触发
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
    // logger->info("TimerQueue::handleRead() {} at {}", howmany, now.toString());
    if (n != sizeof(howmany)) {
        logger->error("TimerQueue::handleRead() reads {} bytes instead of 8", n);
    }
}

void resetTimerfd(int timerfd, Timestamp expiration) { // 重置定时器的超时时间
    struct itimerspec newValue; // 定时器新的超时时间
    struct itimerspec oldValue; // 定时器旧的超时时间
    ::bzero(&newValue, sizeof(newValue));
    ::bzero(&oldValue, sizeof(oldValue));
    newValue.it_value = howMuchTimeFromNow(expiration); // 定时器超时时间
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue); // 设置定时器超时时间
    if (ret) {
        logger->error("timerfd_settime()");
    }
}

TimerQueue::TimerQueue(EventLoop *loop)
        : loop_(loop),
          timerfd_(createTimerfd()),
          timerfdChannel_(loop, timerfd_),
          timers_(),
          callingExpiredTimers_(false) {
    timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue() {
    timerfdChannel_.disableAll();
    timerfdChannel_.remove();
    ::close(timerfd_);
}

TimerId TimerQueue::addTimer(TimerCallback cb, Timestamp when, double interval) {
    auto timer = std::make_shared<Timer>(std::move(cb), when, interval);
    TimerId timerId(timer, timer->sequence());
    
    // 使用lambda来捕获shared_timer
    loop_->runInLoop([this, t = std::move(timer)]() {
        this->addTimerInLoop(t);
    });
    
    return timerId;
}

void TimerQueue::cancel(const TimerId& timerId) {
    loop_->runInLoop([this, timerId] { cancelInLoop(timerId); });
}

void TimerQueue::addTimerInLoop(const std::shared_ptr<Timer>& timer) {
    loop_->assertInLoopThread();
    auto expiration = timer->expiration();
    bool earliestChanged = insert(timer);
    if (earliestChanged) {
        resetTimerfd(timerfd_, expiration);
    }
}

void TimerQueue::cancelInLoop(const TimerId& timerId) {
    loop_->assertInLoopThread();
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    auto it = activeTimers_.find(timer);
    if (it != activeTimers_.end()) {
        size_t n = timers_.erase(Entry(it->first->expiration(), it->first)); // 删除对应的定时器
        if (n != 1) {
            logger->error("TimerQueue::cancelInLoop() erase {} timers instead of 1", n);
        }
        activeTimers_.erase(it); // 删除对应的活跃定时器
    } else if (callingExpiredTimers_) {
        cancelingTimers_.insert(timer); // 如果定时器已经到期，并且正在调用回调函数，则延迟到回调函数返回时删除
    }
}

void TimerQueue::handleRead() {
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now); // 清除该事件，避免一直触发
    std::vector<TimerQueue::Entry> expired;
    getExpired(now, expired); // 获取超时定时器列表
    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    for (const Entry &it : expired) {
        it.second->run(); // 调用定时器回调函数
    }
    callingExpiredTimers_ = false;
    reset(expired, now); // 重置周期性定时器
}

void TimerQueue::getExpired(Timestamp now, std::vector<TimerQueue::Entry>& expired) {
    Entry sentry(now, nullptr);
    auto end = timers_.lower_bound(sentry); // 返回第一个未到期的定时器
    std::copy(timers_.begin(), end, back_inserter(expired)); // 将到期定时器插入到expired中
    timers_.erase(timers_.begin(), end); // 从timers_中删除到期定时器
    for (const Entry &it : expired) {
        ActiveTimer timer(it.second, it.second->sequence());
        size_t n = activeTimers_.erase(timer); // 从activeTimers_中删除到期定时器
        if (n != 1) {
            logger->error("TimerQueue::getExpired() erase {} timers instead of 1", n);
        }
    }
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now) {
    Timestamp nextExpire;
    for (const Entry& it : expired) {
        ActiveTimer timer(it.second, it.second->sequence()); // 重启定时器
        if (it.second->repeat() && cancelingTimers_.find(timer) == cancelingTimers_.end()) {
            it.second->restart(now); // 重启定时器
            insert(it.second); // 插入到timers_中
        }
    }
    if (!timers_.empty()) {
        nextExpire = timers_.begin()->second->expiration();
    }
    if (nextExpire.valid()) {
        resetTimerfd(timerfd_, nextExpire); // 重置定时器的超时时刻
    }
}

bool TimerQueue::insert(const std::shared_ptr<Timer>& timer) {
    loop_->assertInLoopThread();
    bool earliestChanged = false;
    Timestamp when = timer->expiration(); // 超时时刻
    auto it = timers_.begin(); // timers_是一个有序集合，第一个元素就是超时时间最小的定时器
    if (it == timers_.end() || when < it->first) { // 如果timers_为空或者when小于timers_中的最小超时时刻
        earliestChanged = true;
    }
    {
        std::pair<TimerList::iterator, bool> result = timers_.insert(Entry(when, timer)); // 将定时器插入到timers_中
    }
    {
        std::pair<ActiveTimerSet::iterator, bool> result = activeTimers_.insert(ActiveTimer(timer, timer->sequence())); // 将定时器插入到activeTimers_中
    }
    return earliestChanged;
}
