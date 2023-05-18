#pragma once

#include <mutex>
#include "nocopyable.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"

extern std::shared_ptr<spdlog::logger> logger;

class Logger : public noncopyable {
  public:
    // 获取日志记录器的单例实例
    static std::shared_ptr<spdlog::logger> instance();
    
  private:
    // 私有构造函数，防止外部创建实例
    Logger() = default;
    
    // 初始化日志记录器
    static void init();
    
    // 用于确保初始化只执行一次的标志
    static std::once_flag init_flag_;
};
