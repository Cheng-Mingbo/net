//
// Created by Cheng MingBo on 2023/5/13.
//

#ifndef LIBNET_TIMER_ID_H
#define LIBNET_TIMER_ID_H

#include <memory>
#include <utility>
#include "timer.h"
#include "copyable.h"
#include "logger.h"

class Timer;

class TimerId : public copyable {
  public:
    TimerId() : timer_(nullptr)
                , sequence_(0) { }
    
    TimerId(std::shared_ptr<Timer> timer, int64_t seq) : timer_(std::move(timer))
                                         , sequence_(seq) { }
    
    friend class TimerQueue;
    ~TimerId() {
        if (timer_) {
            //logger->info("Destructor of TimerId for Timer with sequence {}", sequence_);
        }
    }
  
  private:
    std::shared_ptr<Timer> timer_;
    int64_t sequence_;
};

#endif //LIBNET_TIMER_ID_H
