//
// Created by Cheng MingBo on 2023/5/15.
//
#include "event_loop_thread.h"
#include "event_loop.h"
#include <utility>


void EventLoopThread::threadFunc() {
    EventLoop loop;
    if (thread_init_callback_) {
        thread_init_callback_(&loop);
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }
    loop.loop();
    std::lock_guard<std::mutex> lock(mutex_);
    loop_ = nullptr;
}

EventLoopThread::EventLoopThread(EventLoopThread::ThreadInitCallback  cb, std::string  name) :
        loop_(nullptr)
        , exiting_(false)
        , name_(std::move(name))
        , thread_init_callback_(std::move(cb))
        , thread_(&EventLoopThread::threadFunc, this)
{}

EventLoop* EventLoopThread::startLoop() {
    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { return loop_ != nullptr; });
        loop = loop_;
    }
    return loop;
}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != nullptr) {
        loop_->quit();
    }
}
