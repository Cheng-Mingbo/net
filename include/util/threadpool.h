//
// Created by Cheng MingBo on 2023/5/12.
//
#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <thread>
#include <vector>
#include <functional>
#include <random>
#include <future>
#include <type_traits>
#include "Queue.h"

class ThreadPool {
  public:
    explicit ThreadPool(std::size_t threadNums = std::thread::hardware_concurrency()
            << 1, bool enableWorkStealing = true);
    
    ~ThreadPool() {
        stop_ = true;
        for (auto& queue : queues_) {
            queue.stop();
        }
    }
    
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using return_type = std::invoke_result_t<F, Args...>;
        
        auto task = std::make_shared<std::packaged_task<return_type()>>([f = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable {
            return std::invoke(std::move(f), std::move(args)...);
        });
        
        auto future = task->get_future();
        auto idx = dis_(gen_);
        queues_[idx].push([task]() { (*task)(); });
        //queues_[next_++ % threadNums_].push([task]() { (*task)(); });
        
        if (getItems() > threadNums_ * 6) {
            expand(threadNums_ * 2);
        }
        
        return future;
    }
    
    std::size_t getItems() {
        std::size_t items = 0;
        for (auto& queue : queues_) {
            items += queue.size();
        }
        return items;
    }
  
  private:
    void expand(std::size_t num);
    
    void adjustPoolSize();
    
    void threadFunc(int idx);
    
    static std::size_t MAX_THREAD_NUMS;
    std::vector<std::jthread> threads_;
    int next_{0};
    std::vector<Queue<std::function<void()>>> queues_;
    std::atomic<std::size_t> threadNums_;
    bool enableWorkStealing_;
    std::random_device rd_;
    std::uniform_int_distribution<> dis_;
    std::mt19937 gen_;
    std::atomic<bool> stop_;
};



#endif //THREADPOOL_THREADPOOL_H
