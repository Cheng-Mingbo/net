//
// Created by Cheng MingBo on 2023/5/14.
//
#include "acceptor.h"
#include "event_loop.h"
#include "inet_address.h"
#include "logger.h"

void newConnection(int sockfd, const InetAddress& peerAddr) {
    logger->info("newConnection(): accepted a new connection from {}", peerAddr.toIpPort());
    ::write(sockfd, "How are you?\n", 13);
    ::close(sockfd);
}

int main() {
    EventLoop loop;
    InetAddress listenAddr(8080);
    Acceptor acceptor(&loop, listenAddr, true);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();

    loop.loop();
}