//
// Created by Cheng MingBo on 2023/5/14.
//


#include "server.h"
#include "acceptor.h"
#include "event_loop.h"
#include "threadpool.h"
#include "logger.h"
#include <random>
#include <utility>

struct sockaddr_in getLocalAddr(int sockfd) {
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    if (::getsockname(sockfd, reinterpret_cast<sockaddr*>(&localaddr), &addrlen) < 0) {
        logger->error("sockets::getLocalAddr");
    }
    
    return localaddr;
}

void Server::newConnection(int sockfd, const InetAddress& peerAddr) {
    loop_->assertInLoopThread();
    EventLoop* ioLoop = thread_pool_->getNextLoop();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", peerAddr.toIpPort().c_str(), next_++);
    std::string connName = name_ + buf;
    logger->info("Server::newConnection [" + name_ + "] - new connection [" + connName + "] from " + peerAddr.toIpPort());
    InetAddress localAddr(getLocalAddr(sockfd));
    connectionPtr conn = std::make_shared<Connection>(ioLoop, connName, sockfd, localAddr, peerAddr);
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&Server::removeConnection, this, std::placeholders::_1));
    ioLoop->runInLoop([conn] { conn->connectEstablished(); });
}

Server::Server(EventLoop* loop, const InetAddress& listen_addr, std::string name) :
        loop_(loop)
        , threadNum_(0)
        , next_(0)
        , name_(std::move(name))
        , acceptor_(std::make_unique<Acceptor>(loop, listen_addr))
        , thread_pool_(std::make_unique<EventLoopThreadPool>(loop, name)) {
    acceptor_
            ->setNewConnectionCallback(std::bind(&Server::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}


void Server::start() {
    thread_pool_->start(threadInitCallback_);
    loop_->runInLoop([capture0 = acceptor_.get()] { capture0->listen(); });
}

void Server::removeConnection(const connectionPtr& conn) {
    loop_->runInLoop([this, conn] { removeConnectionInLoop(conn); });
}

void Server::removeConnectionInLoop(const connectionPtr& conn) {
    loop_->assertInLoopThread();
    size_t n = connections_.erase(conn->name());
    (void) n;
    assert(n == 1);
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(
            std::bind(&Connection::connectDestroyed, conn));
}

void Server::setThreadNum(int num_threads) {
    thread_pool_->setThreadNum(num_threads);
}



