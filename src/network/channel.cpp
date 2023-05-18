//
// Created by Cheng MingBo on 2023/5/13.
//
#include <poll.h>

#include "channel.h"
#include "logger.h"
#include "event_loop.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
        : loop_(loop)
          , fd_(fd)
          , events_(0)
          , revents_(0)
          , index_(-1)
          , tied_(false)
          , eventHandling_(false)
          , addedToLoop_(false) {
    
}

Channel::~Channel() {
}

void Channel::tie(const std::shared_ptr<void>& obj) {
    tie_ = obj;
    tied_ = true;
}

void Channel::update() {
    addedToLoop_ = true;
    loop_->updateChannel(this);
}

void Channel::remove() {
    addedToLoop_ = false;
    loop_->removeChannel(this);
}

void Channel::handleEventWithGuard(Timestamp receiveTime) {
    eventHandling_ = true;
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) { // 对端关闭连接
        if (closeCallback_) closeCallback_();
    }
    
    if (revents_ & (POLLERR | POLLNVAL)) { // POLLERR 表示发生错误
        if (errorCallback_) errorCallback_();
    }
    
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) { // POLLPRI 表示有紧急数据可读
        if (readEventCallback_) readEventCallback_(receiveTime);
    }
    
    if (revents_ & POLLOUT) { // POLLOUT 表示有数据可写
        if (writeCallback_) writeCallback_();
    }
    eventHandling_ = false;
}

void Channel::handleEvent(Timestamp receiveTime) {
    std::shared_ptr<void> guard;
    if (tied_) {
        guard = tie_.lock();
        if (guard) {
            handleEventWithGuard(receiveTime);
        }
    } else {
        handleEventWithGuard(receiveTime);
    }
    
}


