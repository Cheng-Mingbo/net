//
// Created by Cheng MingBo on 2023/5/13.
//
#include <fcntl.h>
#include "socket.h"
#include "inet_address.h"
#include "logger.h"


Socket::~Socket() {
    ::close(sockfd_);
}

bool Socket::getTcpInfo(tcp_info* tcpi) const {
    socklen_t len = sizeof(*tcpi);
    std::memset(tcpi, 0, len);
    return ::getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
}

void Socket::setNonBlocking() {
    int flags = ::fcntl(sockfd_, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd_, F_SETFL, flags);
    if (ret < 0) {
        logger->error("Socket::setNonBlocking");
    }
}

bool Socket::getTcpInfoString(char *buf, int len) const {
    struct tcp_info tcpi;
    bool ok = getTcpInfo(&tcpi);
    if (ok)
    {
        snprintf(buf, len, "unrecovered=%u "
                           "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
                           "lost=%u retrans=%u rtt=%u rttvar=%u "
                           "sshthresh=%u cwnd=%u total_retrans=%u",
                tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
                tcpi.tcpi_rto,          // Retransmit timeout in usec
                tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
                tcpi.tcpi_snd_mss,
                tcpi.tcpi_rcv_mss,
                tcpi.tcpi_lost,         // Lost packets
                tcpi.tcpi_retrans,      // Retransmitted packets out
                tcpi.tcpi_rtt,          // Smoothed round trip time in usec
                tcpi.tcpi_rttvar,       // Medium deviation
                tcpi.tcpi_snd_ssthresh,
                tcpi.tcpi_snd_cwnd,
                tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
    }
    return ok;
}


void Socket::bindAddress(const InetAddress& addr) {
    int ret = ::bind(sockfd_, addr.getSockAddr(), sizeof(*addr.getSockAddr()));
    if (ret < 0) {
        logger->error("Socket::bindAddress");
    }
}

void Socket::listen() {
    int ret = ::listen(sockfd_, SOMAXCONN);
    if (ret < 0) {
        logger->error("Socket::listen");
    }
}

int Socket::accept(InetAddress* peeraddr) {
    int connfd = ::accept(sockfd_, peeraddr->getSockAddr(), peeraddr->getSockAddrLen());
    if (connfd < 0) {
        int savedErrno = errno;
        logger->error("Socket::accept, errno: {} info: {}", savedErrno, strerror(savedErrno));
    }
    return connfd;
}

void Socket::shutdownWrite() {
    if (::shutdown(sockfd_, SHUT_WR) < 0) {
        logger->error("Socket::shutdownWrite");
    }
}

void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReusePort(bool on) {
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
            &optval, static_cast<socklen_t>(sizeof optval));
    if (ret < 0 && on) {
        logger->error("Socket::setReusePort");
    }
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
            &optval, static_cast<socklen_t>(sizeof optval));
}





