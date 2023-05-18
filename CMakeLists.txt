cmake_minimum_required(VERSION 3.22)
project(libnet)

set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/network)
include_directories(${PROJECT_SOURCE_DIR}/include/util)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
# 启用测试
enable_testing()
# 添加tests子目录
add_subdirectory(${PROJECT_SOURCE_DIR}/tests)

add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/spdlog)

add_executable(libnet main.cpp)

target_link_libraries(libnet spdlog::spdlog network util)
