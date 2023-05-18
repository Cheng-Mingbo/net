//
// Created by Cheng MingBo on 2023/5/12.
//

#ifndef THREADPOOL_QUEUE_H
#define THREADPOOL_QUEUE_H
#include <type_traits>
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T> requires std::is_move_assignable_v<T>
class Queue {
  public:
    template<typename U>
    void push(U&& value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.emplace(std::forward<U>(value));
        }
        cond_.notify_one();
    }
    
    bool try_push(const T& value) {
        {
            std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);
            if (!lock || stop_) {
                return false;
            }
            queue_.push(value);
        }
        cond_.notify_one();
        return true;
    }
    
    bool pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { return !queue_.empty() || stop_; });
        if (stop_ && queue_.empty()) {
            return false;
        }
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }
    
    bool try_pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);
        if (!lock || queue_.empty()) {
            return false;
        }
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }
    
    bool try_pop_if(T& value, bool (* predict)(T&) = nullptr) {
        std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);
        if (!lock || queue_.empty()) {
            return false;
        }
        if (predict && !predict(queue_.front())) {
            return false;
        }
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }
    
    std::size_t size() const noexcept {
        //std::scoped_lock<std::mutex> lock(mutex_);
        return queue_.size();
    }
    
    bool empty() const noexcept {
        std::scoped_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    
    void stop() noexcept {
        {
            std::scoped_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        cond_.notify_all();
    }
  
  
  private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    bool stop_{ false };
};

#endif //THREADPOOL_QUEUE_H
