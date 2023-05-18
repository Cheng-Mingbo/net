//
// Created by Cheng MingBo on 2023/5/13.
//

#include <cstring>
#include <arpa/inet.h>
#include "inet_address.h"
#include "logger.h"


static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;

InetAddress::InetAddress(uint16_t port, bool loopbackOnly) {
    std::memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
    addr_.sin_addr.s_addr = htobe32(ip);
    addr_.sin_port = htobe16(port);
    addr_len_ = sizeof addr_;
}

InetAddress::InetAddress(std::string ip, uint16_t port) {
    std::memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htobe16(port);
    if (::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) <= 0) {
       logger->error("InetAddress::InetAddress() error: %s", strerror(errno));
    }
    addr_len_ = sizeof addr_;
}

std::string InetAddress::toIpPort() const noexcept {
    char buf[64] = "";
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    uint16_t port = ntohs(addr_.sin_port);
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), ":%u", port);
    return buf;
}

uint32_t InetAddress::ipv4NetEndian() const noexcept {
    return addr_.sin_addr.s_addr;
}

uint16_t InetAddress::portNetEndian() const noexcept {
    return addr_.sin_port;
}