//
// Created by Cheng MingBo on 2023/5/13.
//
#include "timer.h"

std::atomic<int64_t> Timer::s_numCreated_;

void Timer::restart(Timestamp now) {
    if (repeat_) {
        expiration_ = addTime(now, interval_);
    } else {
        expiration_ = Timestamp::invalid();
    }
}