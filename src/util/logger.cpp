//
// Created by Cheng MingBo on 2023/5/13.
//

#include <memory>
#include "logger.h"
#include "spdlog/async.h"

std::once_flag Logger::init_flag_;

std::shared_ptr<spdlog::logger> Logger::instance() {
    std::call_once(init_flag_, init);
    
    return spdlog::get("logger");
}

void Logger::init() {
    spdlog::init_thread_pool(8192, 1);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/mylog.txt", 1024*1024*10, 3);
    std::vector<spdlog::sink_ptr> sinks {stdout_sink, rotating_sink};
    auto logger = std::make_shared<spdlog::async_logger>("logger", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    spdlog::register_logger(logger);
}

std::shared_ptr<spdlog::logger> logger = Logger::instance();