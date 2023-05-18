//
// Created by Cheng MingBo on 2023/5/14.
//

#include "threadpool.h"

std::size_t ThreadPool::MAX_THREAD_NUMS = std::thread::hardware_concurrency() << 4;

ThreadPool::ThreadPool(std::size_t threadNums, bool enableWorkStealing)
        : threadNums_(threadNums)
          , stop_(false)
          , queues_(MAX_THREAD_NUMS + 1)
          , enableWorkStealing_(enableWorkStealing)
          , dis_(0, threadNums - 1)
          , gen_(rd_()) {
    for (int i = 0; i < threadNums_; i++) {
        threads_.emplace_back(&ThreadPool::threadFunc, this, i);
    }
}

void ThreadPool::threadFunc(int idx) {
    while (!stop_) {
        std::function<void()> task;
        if (enableWorkStealing_) {
            for (int i = 0; i < threadNums_; i++) {
                if (queues_[(idx + i) % threadNums_].try_pop(task)) {
                    break;
                }
            }
        }
        else {
            queues_[idx].pop(task);
        }
        if (task == nullptr) {
            queues_[idx].pop(task);
        }
        if (task) {
            task();
        }
    }
}

void ThreadPool::expand(std::size_t num) {
    if (threadNums_ < MAX_THREAD_NUMS && num > threadNums_) {
        threadNums_ = num;
        adjustPoolSize();
    }
}

void ThreadPool::adjustPoolSize() {
    if (threadNums_ < MAX_THREAD_NUMS && threads_.size() < threadNums_) {
        // 添加新的线程
        for (std::size_t i = threads_.size(); i < threadNums_; ++i) {
            threads_.emplace_back(&ThreadPool::threadFunc, this, i);
        }
    }
}
