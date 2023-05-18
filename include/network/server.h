//
// Created by Cheng MingBo on 2023/5/14.
//

#ifndef LIBNET_SERVER_H
#define LIBNET_SERVER_H
#include <map>

#include "nocopyable.h"
#include "inet_address.h"
#include "connection.h"
#include "acceptor.h"
#include "event_loop_threadpool.h"


class Acceptor;
class EventLoop;
using connectionPtr = std::shared_ptr<Connection>;
class Server : public noncopyable {
  public:
    using threadInitCallback = std::function<void(EventLoop*)>;
    using connectionCallback = std::function<void(const connectionPtr&)>;
    using messageCallback = std::function<void(const connectionPtr&, Buffer*, Timestamp)>;
    using writeCompleteCallback = std::function<void(const connectionPtr&)>;
    using closeCallback = std::function<void(const connectionPtr&)>;
    
    Server(EventLoop* loop, const InetAddress& listen_addr, std::string  name = "server");
    ~Server() = default;
    EventLoop* getLoop() const { return loop_; }
    
    void setThreadNum(int num_threads);
    void start();
    void setConnectionCallback(const connectionCallback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const messageCallback& cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const writeCompleteCallback& cb) { writeCompleteCallback_ = cb; }
    void setThreadInitCallback(const threadInitCallback& cb) { threadInitCallback_ = cb; }
  
  private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const connectionPtr& conn);
    void removeConnectionInLoop(const connectionPtr& conn);
    
    using connectionMap = std::map<std::string, connectionPtr>;
    EventLoop* loop_;
    int threadNum_;
    int next_;
    std::vector<EventLoop> loops_;
    std::unique_ptr<EventLoopThreadPool> thread_pool_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    connectionMap connections_;
    threadInitCallback threadInitCallback_;
    connectionCallback connectionCallback_;
    messageCallback messageCallback_;
    writeCompleteCallback writeCompleteCallback_;
    
};
#endif //LIBNET_SERVER_H
