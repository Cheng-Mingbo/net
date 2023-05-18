//
// Created by Cheng MingBo on 2023/5/15.
//
#include "event_loop_threadpool.h"

#include <utility>
#include <memory>
#include <random>

EventLoopThreadPool::EventLoopThreadPool(EventLoop* base_loop, std::string  name) :
        base_loop_(base_loop)
        , name_(std::move(name))
        , started_(false)
        , num_threads_(0)
        , next_(0) { }
        
        
void EventLoopThreadPool::start(const EventLoopThreadPool::ThreadInitCallback& cb) {
    started_ = true;
    for (int i = 0; i < num_threads_; ++i) {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
        threads_.emplace_back(std::make_unique<EventLoopThread>(cb, std::string(buf)));
        loops_.push_back(threads_.back()->startLoop());
    }
    if (num_threads_ == 0 && cb) {
        cb(base_loop_);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    static std::random_device rd_;
    static std::uniform_int_distribution<> dis_(0, num_threads_ - 1);
    static std::mt19937 gen_(rd_());
    
    int idx = dis_(gen_);
    return loops_[idx];
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops() {
    return loops_;
}
