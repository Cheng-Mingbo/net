//
// Created by Cheng MingBo on 2023/5/13.
//
#ifndef LIBNET_EVENT_LOOP_H
#define LIBNET_EVENT_LOOP_H
#include <functional>
#include <atomic>
#include <thread>
#include <memory>
#include <any>
#include <mutex>
#include "nocopyable.h"
#include "Timestamp.h"
#include "timer_id.h"

class Channel;
class EPoll;
class TimerQueue;

class EventLoop : noncopyable {
  public:
    using Functor = std::function<void()>;
    
    EventLoop();
    ~EventLoop();
    
    void loop();
    void quit();
    
    Timestamp pollReturnTime() const { return pollReturnTime_; }
    uint64_t iteration() const { return iteration_; }
    void runInLoop(Functor cb);
    void queueInLoop(Functor cb);
    std::size_t queueSize() const;
    
    TimerId runAt(Timestamp time, Timer::TimerCallback cb);
    TimerId runAfter(double delay, Timer::TimerCallback cb);
    TimerId runEvery(double interval, Timer::TimerCallback cb);
    void cancel(const TimerId& timerId);
    
    
    void wakeup();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);
    
    bool isInLoopThread() const { return threadId_ == std::this_thread::get_id(); }
    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }
    
    bool eventHandling() const { return eventHandling_; }
    void setContext(const std::any& context) { context_ = context; } // for HttpContext
    [[nodiscard]] const std::any& getContext() const { return context_; } // for HttpContext
    std::any* getMutableContext() { return &context_; } // for HttpContext
    static EventLoop* getEventLoopOfCurrentThread(); // 获取当前线程的EventLoop
  private:
    void abortNotInLoopThread(); // 打印错误信息并退出程序
    void handleRead(); // wakeupChannel_的可读事件回调
    void doPendingFunctors(); // 执行回调
    
    using ChannelList = std::vector<Channel*>;
    std::atomic<bool> looping_; // 是否处于事件循环中
    std::atomic<bool> quit_; // 是否退出事件循环
    std::atomic<bool> eventHandling_; // 是否处于事件处理中
    std::atomic<bool> callingPendingFunctors_; // 是否正在调用pendingFunctors_
    uint64_t iteration_; // 当前事件循环已经执行的轮数
    std::thread::id threadId_; // 事件循环所在的线程ID
    Timestamp pollReturnTime_; // 最近一次poll函数返回的时间戳
    std::unique_ptr<EPoll> poller_; // IO复用的封装
    std::unique_ptr<TimerQueue> timerQueue_; // 定时器队列
    int wakeupFd_; // 用于eventfd
    std::unique_ptr<Channel> wakeupChannel_; // 用于eventfd
    std::any context_; // 事件循环上下文
    
    ChannelList activeChannels_; // 当前活跃的Channel
    Channel* currentActiveChannel_; // 当前正在处理的Channel
    mutable std::mutex mutex_; // 保护activeChannels_
    std::vector<Functor> pendingFunctors_; // 任务队列
};


#endif //LIBNET_EVENT_LOOP_H
