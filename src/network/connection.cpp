//
// Created by Cheng MingBo on 2023/5/14.
//
#include <err.h>
#include "connection.h"
#include "event_loop.h"


Connection::Connection(EventLoop* loop, const std::string& name, int sockfd, const InetAddress& local_addr, const InetAddress& peer_addr)
        : owner_loop(loop)
          , name_(name)
          , state_(state::kConnecting)
          , reading_(true)
          , socket_(new Socket(sockfd))
          , channel_(new Channel(loop, sockfd))
          , local_addr_(local_addr)
          , peer_addr_(peer_addr)
          , read_buffer_(std::make_unique<Buffer>())
          , write_buffer_(std::make_unique<Buffer>()) {
    channel_->setReadCallback(std::bind(&Connection::handleRead, this, std::placeholders::_1));
    channel_->setWriteCallback(std::bind(&Connection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&Connection::handleClose, this));
    channel_->setErrorCallback(std::bind(&Connection::handleError, this));
    socket_->setKeepAlive(true);
}


void Connection::handleRead(Timestamp receive_time) {
    owner_loop->assertInLoopThread();
    int saved_errno = 0;
    ssize_t n = read_buffer_->readFd(channel_->fd(), &saved_errno);
    if (n > 0) {
        message_callback_(shared_from_this(), read_buffer_.get(), receive_time);
    }
    else if (n == 0) {
        handleClose();
    }
    else {
        errno = saved_errno;
        logger->error("Connection::handleRead");
        handleError();
    }
}

void Connection::handleWrite() {
    owner_loop->assertInLoopThread();
    if (channel_->isWriting()) {
        ssize_t n = ::write(channel_->fd(), write_buffer_->peek(), write_buffer_->readableBytes());
        if (n > 0) {
            write_buffer_->retrieve(n);
            if (write_buffer_->readableBytes() == 0) {
                channel_->disableWriting();
                if (write_complete_callback_) {
                    owner_loop->queueInLoop(std::bind(write_complete_callback_, shared_from_this()));
                }
                if (state_ == state::kDisconnecting) {
                    shutdownInLoop();
                }
            }
            else {
                logger->error("Connection::handleWrite");
            }
        }
        else {
            logger->error("Connection::handleWrite");
        }
    }
}

void Connection::handleClose() {
    owner_loop->assertInLoopThread();
    logger->info("Connection::handleClose fd = {}", channel_->fd());
    state_ = state::kDisconnected;
    channel_->disableAll();
    connection_callback_(shared_from_this());
    close_callback_(shared_from_this());
}

void Connection::handleError() {
    logger->error("Connection::handleError [{}]: {}", name_, strerror(errno));
}

void Connection::sendInLoop(const std::string& message) {
    sendInLoop(message.data(), message.size());
}

void Connection::sendInLoop(const void* data, size_t len) {
    owner_loop->assertInLoopThread();
    ssize_t nwrote = 0; // bytes written
    size_t remaining = len; // remaining bytes to write
    bool fault_error = false;
    if (state_ == state::kDisconnected) {
        return;
    }
    // if no thing in output queue, try writing directly
    if (!channel_->isWriting() && write_buffer_->readableBytes() == 0) {
        nwrote = ::write(channel_->fd(), data, len); // 已经写入的字节数
        if (nwrote >= 0) {
            remaining = len - nwrote; // 剩余的字节数
            if (remaining == 0 && write_complete_callback_) { // 写完了
                owner_loop->queueInLoop(std::bind(write_complete_callback_, shared_from_this()));
            }
        }
        else { // nwrote < 0
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                logger->error("Connection::sendInLoop");
                if (errno == EPIPE || errno == ECONNRESET) {
                    fault_error = true;
                }
            }
        }
    }
    if (!fault_error && remaining > 0) {
        size_t old_len = write_buffer_->readableBytes(); // 旧的字节数
        write_buffer_->append(static_cast<const char*>(data) + nwrote, remaining);
        if (!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

void Connection::shutdownInLoop() {
    owner_loop->assertInLoopThread();
    if (!channel_->isWriting()) {
        socket_->shutdownWrite();
    }
}

void Connection::forceCloseInLoop() {
    owner_loop->assertInLoopThread();
    if (state_ == state::kConnected || state_ == state::kDisconnecting) {
        handleClose();
    }
}

void Connection::startReadInLoop() {
    owner_loop->assertInLoopThread();
    if (!reading_ || !channel_->isReading()) {
        channel_->enableReading();
        reading_ = true;
    }
    
}

void Connection::stopReadInLoop() {
    owner_loop->assertInLoopThread();
    if (reading_ || channel_->isReading()) {
        channel_->disableReading();
        reading_ = false;
    }
    
}

bool Connection::getTcpInfo(struct tcp_info* tcpi) const {
    return socket_->getTcpInfo(tcpi);
}

std::string Connection::getTcpInfoString() const {
    char buf[1024];
    buf[0] = '\0';
    socket_->getTcpInfoString(buf, sizeof buf);
    return buf;
}

void Connection::send(const std::string& message) {
    if (state_ == state::kConnected) {
        if (owner_loop->isInLoopThread()) {
            sendInLoop(message);
        }
        else {
            void (Connection::*fp)(const std::string& message) = &Connection::sendInLoop;
            owner_loop->runInLoop(std::bind(fp, this, message));
        }
    }
}

void Connection::send(const void* data, size_t len) {
    if (state_ == state::kConnected) {
        if (owner_loop->isInLoopThread()) {
            sendInLoop(data, len);
        }
        else {
            void (Connection::*fp)(const void* data, size_t len) = &Connection::sendInLoop;
            owner_loop->runInLoop(std::bind(fp, this, data, len));
        }
    }
}

void Connection::send(Buffer* message) {
    if (state_ == state::kConnected) {
        if (owner_loop->isInLoopThread()) {
            sendInLoop(message->peek(), message->readableBytes());
            message->retrieveAll();
        }
        else {
            void (Connection::*fp)(const std::string& message) = &Connection::sendInLoop;
            owner_loop->runInLoop(std::bind(fp, this, message->retrieveAllAsString()));
        }
    }
}


void Connection::startRead() {
    owner_loop->runInLoop(std::bind(&Connection::startReadInLoop, this));
}

void Connection::stopRead() {
    owner_loop->runInLoop(std::bind(&Connection::stopReadInLoop, this));
}

void Connection::connectEstablished() {
    owner_loop->assertInLoopThread();
    setState(state::kConnected);
    channel_->enableReading();
    channel_->tie(shared_from_this());
    connection_callback_(shared_from_this());
}

void Connection::connectDestroyed() {
    owner_loop->assertInLoopThread();
    if (state_ == state::kConnected) {
        setState(state::kDisconnected);
        channel_->disableAll();
        connection_callback_(shared_from_this());
    }
    channel_->remove();
}

