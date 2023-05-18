//
// Created by Cheng MingBo on 2023/5/15.
//

#ifndef LIBNET_EVENT_LOOP_THREAD_H
#define LIBNET_EVENT_LOOP_THREAD_H
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "nocopyable.h"

class EventLoop;

class EventLoopThread : public noncopyable {
  public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    EventLoopThread(ThreadInitCallback  cb = ThreadInitCallback(), std::string  name = std::string());
    ~EventLoopThread();
    EventLoop* startLoop();
  private:
    EventLoop* loop_;
    bool exiting_;
    std::string name_;
    std::jthread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback thread_init_callback_;
    
    void threadFunc();
};


#endif //LIBNET_EVENT_LOOP_THREAD_H
