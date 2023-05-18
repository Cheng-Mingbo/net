//
// Created by Cheng MingBo on 2023/5/13.
//

#ifndef LIBNET_EPOLL_H
#define LIBNET_EPOLL_H

#include <unordered_map>
#include <vector>

#include "nocopyable.h"


class EventLoop;
class Channel;
class Timestamp;

class EPoll : public noncopyable {
  public:
    using ChannelList = std::vector<Channel*>;
    EPoll(EventLoop* loop);
    ~EPoll();
    
    Timestamp poll(int timeoutMs, ChannelList* activeChannels); // 调用epoll_wait
    void updateChannel(Channel* channel); // 更新channel
    void removeChannel(Channel* channel); // 移除channel
    bool hasChannel(Channel* channel) const; // 判断是否有channel
    
  private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const; // 填充活跃的channel
    void update(int operation, Channel* channel); // 更新epoll中的channel
    
    using EventList = std::vector<struct epoll_event>;
    static const int kInitEventListSize = 16; // 初始事件列表大小
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;
    EventLoop* ownerLoop_;
    int epollfd_;
    EventList events_;
};

#endif //LIBNET_EPOLL_H
