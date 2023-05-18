AUX_SOURCE_DIRECTORY(. UTIL_SRC)
add_library(util ${UTIL_SRC})
target_link_libraries(util PUBLIC spdlog::spdlog)
target_include_directories(util PUBLIC ${PROJECT_SOURCE_DIR}/include/util)