//
// Created by Cheng MingBo on 2023/5/13.
//

#ifndef LIBNET_INET_ADDRESS_H
#define LIBNET_INET_ADDRESS_H

#include <netinet/in.h>
#include <string>

#include "copyable.h"

class InetAddress : public copyable {
  public:
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false);
    InetAddress(std::string ip, uint16_t port);
    explicit InetAddress(const struct sockaddr_in& addr) : addr_(addr), addr_len_(sizeof addr) { }
    
    [[nodiscard]] std::string toIpPort() const noexcept;
    [[nodiscard]] uint32_t ipv4NetEndian() const noexcept;
    [[nodiscard]] uint16_t portNetEndian() const noexcept;
    
    
    [[nodiscard]] const sockaddr* getSockAddr() const noexcept { return reinterpret_cast<const sockaddr*>(&addr_); }
    sockaddr* getSockAddr() noexcept{ return reinterpret_cast<sockaddr*>(&addr_); }
    socklen_t* const getSockAddrLen() noexcept { return reinterpret_cast<socklen_t* const>(&addr_len_); }
    void setSockAddrInet(const sockaddr_in& addr) noexcept { addr_ = addr; }
    
  private:
    sockaddr_in addr_;
    socklen_t addr_len_;
};




#endif //LIBNET_INET_ADDRESS_H
