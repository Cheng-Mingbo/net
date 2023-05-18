//
// Created by Cheng MingBo on 2023/5/14.
//

#ifndef LIBNET_CONNECTION_H
#define LIBNET_CONNECTION_H
#include <memory>
#include <any>

#include "nocopyable.h"
#include "socket.h"
#include "buffer.h"
#include "channel.h"
#include "Timestamp.h"
#include "inet_address.h"

class EventLoop;

class Connection : public noncopyable, public std::enable_shared_from_this<Connection>{
  public:
    using connectionPtr = std::shared_ptr<Connection>;
    using connectionCallback = std::function<void(const connectionPtr&)>;
    using messageCallback = std::function<void(const connectionPtr&, Buffer*, Timestamp)>;
    using writeCompleteCallback = std::function<void(const connectionPtr&)>;
    using closeCallback = std::function<void(const connectionPtr&)>;
    
    
    explicit Connection(EventLoop* loop, const std::string& name, int sockfd, const InetAddress& local_addr, const InetAddress& peer_addr);
    
    ~Connection() = default;
    
    EventLoop* getLoop() const { return owner_loop; }
    const std::string& name() const { return name_; }
    const InetAddress& localAddress() const { return local_addr_; }
    const InetAddress& peerAddress() const { return peer_addr_; }
    bool connected() const { return state_ == state::kConnected; }
    bool disconnected() const { return state_ == state::kDisconnected; }
    bool getTcpInfo(struct tcp_info*) const;
    std::string getTcpInfoString() const;
    
    void send(const std::string& message);
    void send(const void* data, size_t len);
    void send(Buffer* message);
    void setTcpNoDelay(bool on);
    
    void startRead();
    void stopRead();
    bool isReading() const { return reading_; };
    
    void setContext(const std::any& context) { context_ = context; }
    const std::any& getContext() const { return context_; }
    std::any* getMutableContext() { return &context_; }
    void setConnectionCallback(const connectionCallback& cb) { connection_callback_ = cb; }
    void setMessageCallback(const messageCallback& cb) { message_callback_ = cb; }
    void setWriteCompleteCallback(const writeCompleteCallback& cb) { write_complete_callback_ = cb; }
    void setCloseCallback(const closeCallback& cb) { close_callback_ = cb; }
    
    Buffer* writeBuffer() { return write_buffer_.get(); }
    Buffer* readBuffer() { return read_buffer_.get(); }
    
    void connectEstablished();
    void connectDestroyed();
  private:
    enum class state { kConnecting, kConnected, kDisconnecting, kDisconnected };
    void handleRead(Timestamp receive_time);
    void handleWrite();
    void handleClose();
    void handleError();
    
    void sendInLoop(const std::string& message);
    void sendInLoop(const void* data, size_t len);
    void shutdownInLoop();
    
    void forceCloseInLoop();
    
    void setState(state s) { state_ = s; }
    
    void startReadInLoop();
    void stopReadInLoop();
    
    EventLoop* owner_loop;
    std::string name_;
    const InetAddress local_addr_;
    const InetAddress peer_addr_;
    connectionCallback connection_callback_;
    messageCallback message_callback_;
    writeCompleteCallback write_complete_callback_;
    closeCallback close_callback_;
    std::any context_;
    bool reading_;
    state state_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Buffer> read_buffer_;
    std::unique_ptr<Buffer> write_buffer_;
    std::unique_ptr<Channel> channel_;
};
#endif //LIBNET_CONNECTION_H
