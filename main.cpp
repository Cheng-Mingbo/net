//
// Created by Cheng MingBo on 2023/5/13.
//
#include <iostream>
#include "socket.h"
#include "inet_address.h"

int main() {
    // 创建一个套接字
    Socket socket(520);
    
    // 设置套接字的一些选项
    socket.setReuseAddr(true);
    socket.setReusePort(true);
    socket.setKeepAlive(true);
    
    // 绑定到一个本地地址
    InetAddress address(8080); // 用你的端口号替换8080
    socket.bindAddress(address);
    
    // 开始监听
    socket.listen();
    
    // 接受一个连接
    InetAddress peerAddress;
    int connectionSocket = socket.accept(&peerAddress);
    
    if (connectionSocket >= 0) {
        std::cout << "Received a connection from " << peerAddress.toIpPort() << std::endl;
    } else {
        std::cout << "Failed to accept a connection" << std::endl;
    }
    
    // 最后，关闭套接字
    socket.shutdownWrite();
    
    return 0;
}
