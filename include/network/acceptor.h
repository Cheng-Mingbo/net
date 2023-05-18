//
// Created by Cheng MingBo on 2023/5/14.
//

#ifndef LIBNET_ACCEPTOR_H
#define LIBNET_ACCEPTOR_H
#include <functional>

#include "channel.h"
#include "socket.h"
#include "nocopyable.h"

class EventLoop;
class InetAddress;

class Acceptor : public noncopyable {
  public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;
    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport = false);
    ~Acceptor();
    
    void setNewConnectionCallback(const NewConnectionCallback& cb) {
        newConnectionCallback_ = cb;
    }
    
    void listen();
    bool isListenning() const { return listenning_; }
    
    
  private:
    void handleRead();
    
    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
    int idleFd_; // used to accept new connection when listenningFd_ reach the limit
};


#endif //LIBNET_ACCEPTOR_H
