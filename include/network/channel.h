//
// Created by Cheng MingBo on 2023/5/13.
//

#ifndef LIBNET_CHANNEL_H
#define LIBNET_CHANNEL_H
#include <functional>
#include <memory>

#include "nocopyable.h"
#include "Timestamp.h"

class EventLoop;


class Channel : public noncopyable {
  public:
    using EventCallback = std::function<void()>; // 事件回调函数
    using ReadEventCallback = std::function<void(Timestamp)>; // 读事件回调函数
    
    Channel(EventLoop* loop, int fd);
    ~Channel();
    
    void handleEvent(Timestamp receiveTime);
    void handleEventWithGuard(Timestamp receiveTime);
    
    void setReadCallback(ReadEventCallback cb) { readEventCallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }
    
    int fd() const { return fd_; }
    int events() const { return events_; }
    void setRevents(int events) { revents_ = events; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }
    
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }
    
    int index() { return index_; }
    void setIndex(int idx) { index_ = idx; }
    void tie(const std::shared_ptr<void>&);
    
    EventLoop* ownerLoop() { return loop_; }
    void remove();
    
  private:
    void update();
    
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
    
    EventLoop* loop_;
    const int fd_;
    int events_; // 关注的事件
    int revents_; // 目前活动的事件
    int index_; // used by Poller
    
    std::weak_ptr<void> tie_; // 用于绑定生命周期
    bool tied_; // 是否绑定生命周期
    bool addedToLoop_; // 是否已经添加到EventLoop中
    bool eventHandling_; // 是否正在处理事件
    
    ReadEventCallback readEventCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};


#endif //LIBNET_CHANNEL_H
