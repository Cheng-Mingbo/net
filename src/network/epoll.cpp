//
// Created by Cheng MingBo on 2023/5/13.
//

#include <sys/epoll.h>
#include "epoll.h"
#include "logger.h"
#include "Timestamp.h"
#include "channel.h"

namespace {
    const int kNew = -1; // 新增
    const int kAdded = 1; // 已添加
    const int kDeleted = 2; // 已删除
}

EPoll::EPoll(EventLoop* loop) :
        ownerLoop_(loop)
        , epollfd_(::epoll_create1(EPOLL_CLOEXEC))
        , events_(kInitEventListSize) {
    if (epollfd_ < 0) {
        logger->error("EPoll::EPoll");
    }
}

EPoll::~EPoll() {
    ::close(epollfd_);
}

Timestamp EPoll::poll(int timeoutMs, EPoll::ChannelList* activeChannels) {
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if (numEvents > 0) {
        //logger->info("{} events happened", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    }
    else if (numEvents == 0) {
        logger->info("nothing happened");
    }
    else {
        if (savedErrno != EINTR) {
            errno = savedErrno;
            logger->error("EPoll::poll()");
        }
    }
    return now;
}

void EPoll::fillActiveChannels(int numEvents, EPoll::ChannelList* activeChannels) const {
    for (int i = 0; i < numEvents; ++i) {
        auto* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->setRevents(events_[i].events); // 设置活跃事件
        
        activeChannels->push_back(channel);
    }
}

void EPoll::updateChannel(Channel* channel) {
    const int index = channel->index();
    if (index == kNew || index == kDeleted) {
        int fd = channel->fd();
        if (index == kNew) {
            channels_[fd] = channel;
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else {
        int fd = channel->fd();
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        }
        else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPoll::removeChannel(Channel* channel) {
    int fd = channel->fd();
    channels_.erase(fd);
    if (channel->index() == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

void EPoll::update(int operation, Channel* channel) {
    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            logger->error("epoll_ctl op = EPOLL_CTL_DEL fd = {}", fd);
        }
        else {
            logger->error("epoll_ctl op = {} fd = {}", operation, fd);
        }
    }
}

bool EPoll::hasChannel(Channel* channel) const {
    ChannelMap::const_iterator it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}
