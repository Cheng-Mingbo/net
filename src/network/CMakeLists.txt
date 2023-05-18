AUX_SOURCE_DIRECTORY(. NETWORK_SRC)

add_library(network ${NETWORK_SRC})
target_link_libraries(network PUBLIC spdlog::spdlog)
target_include_directories(network PUBLIC ${PROJECT_SOURCE_DIR}/include/network)
