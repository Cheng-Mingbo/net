//
// Created by Cheng MingBo on 2023/5/13.
//
#include <iostream>
#include <arpa/inet.h>
#include "inet_address.h"

void testInetAddress() {
    // 测试默认构造函数
    InetAddress addr1;
    std::cout << "Address 1: " << addr1.toIpPort() << std::endl;
    
    // 测试构造函数，使用IPv4 loopback地址
    InetAddress addr2(8080, true);
    std::cout << "Address 2: " << addr2.toIpPort() << std::endl;
    
    // 测试构造函数，使用具体的IP和端口
    InetAddress addr3("192.168.1.1", 8080);
    std::cout << "Address 3: " << addr3.toIpPort() << std::endl;
    
    // 测试ipv4NetEndian和portNetEndian方法
    std::cout << "Address 3 IP (net endian): " << addr3.ipv4NetEndian() << std::endl;
    std::cout << "Address 3 port (net endian): " << addr3.portNetEndian() << std::endl;
    
    // 测试getSockAddr和setSockAddrInet方法
    struct sockaddr_in raw_addr;
    raw_addr.sin_family = AF_INET;
    raw_addr.sin_port = htons(9090);
    inet_pton(AF_INET, "10.0.0.1", &(raw_addr.sin_addr));
    
    InetAddress addr4(raw_addr);
    std::cout << "Address 4: " << addr4.toIpPort() << std::endl;
    
    addr4.setSockAddrInet(raw_addr);
    std::cout << "Address 4 (after setSockAddrInet): " << addr4.toIpPort() << std::endl;
}

int main() {
    testInetAddress();
    return 0;
}
