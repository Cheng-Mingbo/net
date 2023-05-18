# 为每个测试文件创建一个可执行文件，并添加到测试中
AUX_SOURCE_DIRECTORY(. TEST_SRCS)
foreach(testSrc ${TEST_SRCS})
    # 从文件名中提取出测试名
    get_filename_component(testName ${testSrc} NAME_WE)

    # 添加可执行文件
    add_executable(${testName} ${testSrc})

    # 链接库到测试可执行文件
    target_link_libraries(${testName} network util)

    # 添加到测试中
    add_test(NAME ${testName} COMMAND ${testName})
endforeach(testSrc)
