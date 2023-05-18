//
// Created by Cheng MingBo on 2023/5/13.
//

#ifndef LIBNET_SOCKET_H
#define LIBNET_SOCKET_H

#include <netinet/tcp.h>
#include "nocopyable.h"

class InetAddress;

class Socket : noncopyable {
  public:
    explicit Socket(int sockfd)
            : sockfd_(sockfd)
    { }
    
    ~Socket();
    int fd() const { return sockfd_; }
    bool getTcpInfo(tcp_info*) const;
    bool getTcpInfoString(char* buf, int len) const;
    
    void bindAddress(const InetAddress& localaddr);
    void listen();
    
    int accept(InetAddress* peeraddr);
    
    void shutdownWrite();
    
    void setNonBlocking();
    
    void setReuseAddr(bool on);
    
    void setReusePort(bool on);
    
    void setKeepAlive(bool on);
  
  private:
    const int sockfd_;
};


#endif //LIBNET_SOCKET_H
