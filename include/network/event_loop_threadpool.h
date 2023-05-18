//
// Created by Cheng MingBo on 2023/5/15.
//

#ifndef LIBNET_EVENT_LOOP_THREADPOOL_H
#define LIBNET_EVENT_LOOP_THREADPOOL_H
#include "nocopyable.h"
#include "event_loop_thread.h"


class EventLoopThreadPool : public noncopyable {
  public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    explicit EventLoopThreadPool(EventLoop* base_loop, std::string name = "threadpool");
    ~EventLoopThreadPool() = default;
    void setThreadNum(int num_threads) { num_threads_ = num_threads; }
    void start(const ThreadInitCallback& cb = ThreadInitCallback());
    EventLoop* getNextLoop();
    
    std::vector<EventLoop*> getAllLoops();
    bool started() const { return started_; }
    [[nodiscard]] const std::string& name() const { return name_; }
    
  private:
    EventLoop* base_loop_; // 接受连接的loop
    std::string name_; // 线程池名字
    bool started_; // 是否已经启动
    int num_threads_; // 线程数
    int next_; // 下一个线程
    std::vector<std::unique_ptr<EventLoopThread>> threads_; // 线程列表
    std::vector<EventLoop*> loops_; // loop列表
};
#endif //LIBNET_EVENT_LOOP_THREADPOOL_H
