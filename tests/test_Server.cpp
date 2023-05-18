#include "server.h"
#include "event_loop.h"
#include "inet_address.h"
#include "logger.h"
#include <stdio.h>

void onConnection(const connectionPtr& conn)
{
    if (conn->connected())
    {
        logger->info("onConnection():  new connection {} from {}\n",
                conn->name().c_str(),
                conn->peerAddress().toIpPort().c_str());
    }
    else
    {
       logger->info("onConnection(): connection {} is down\n", conn->name().c_str());
    }
}

void onMessage(const connectionPtr& conn,
        Buffer* buf,
        Timestamp receiveTime)
{
    
    logger->info("onMessage(): received {} bytes from connection {}\n",
            buf->readableBytes(),
            conn->name().c_str());
    
    conn->send(buf);
    logger->info("onMessage(): sent {} bytes back to connection {}, msg: {}\n",
            buf->readableBytes(),
            conn->name().c_str(), buf->retrieveAllAsString());
}

int main(int argc, char* argv[])
{
    printf("main(): pid = %d\n", getpid());
    
    InetAddress listenAddr(8080);
    EventLoop loop;
    
    Server server(&loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.setThreadNum(8);
    server.start();
    
    loop.loop();
}