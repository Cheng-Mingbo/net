//
// Created by Cheng MingBo on 2023/5/14.
//

#include "event_loop.h"
#include "channel.h"
#include "logger.h"
#include "Timestamp.h"
#include <thread>

EventLoop* g_loop;

void print1() {
    logger->info("This is runAt task!");
}

void print2() {
    logger->info("This is runAfter task!");
}

void print3() {
    logger->info("This is runEvery task!");
}

void threadFunc() {
    g_loop->runAt(Timestamp::now(), print1);
    g_loop->runAfter(2.0, print2);
    g_loop->runEvery(3.0, print3);
}

int main() {
    EventLoop loop;
    g_loop = &loop;
    std::jthread t(threadFunc);
    loop.loop();
    
    return 0;
}