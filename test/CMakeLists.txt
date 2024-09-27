cmake_minimum_required(VERSION 3.10)

include(GoogleTest)

# 递归查找所有测试源文件
file(GLOB_RECURSE cppCoroutine_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*/*test.cpp") #"${PROJECT_SOURCE_DIR}/test/*Test.cpp")

# #########################################
# "make check-tests"
# #########################################
add_custom_target(build-tests COMMAND ${CMAKE_CTEST_COMMAND} --show-only)
add_custom_target(check-tests COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

foreach (cppCoroutine_test_source ${cppCoroutine_TEST_SOURCES})
    # 创建可读的测试名称
    get_filename_component(cppCoroutine_test_filename ${cppCoroutine_test_source} NAME_WE)
    string(REPLACE ".cpp" "" cppCoroutine_test_name ${cppCoroutine_test_filename})

    # 单独添加测试目标并作为 "make check-tests" 的一部分
    add_executable(${cppCoroutine_test_name} ${cppCoroutine_test_source} "${PROJECT_SOURCE_DIR}/tools/backtrace.cpp")
    add_backward(${cppCoroutine_test_name})
    add_dependencies(build-tests ${cppCoroutine_test_name})
    add_dependencies(check-tests ${cppCoroutine_test_name})

    gtest_discover_tests(${cppCoroutine_test_name}
        EXTRA_ARGS
        --gtest_output=xml:${CMAKE_BINARY_DIR}/test/${cppCoroutine_test_name}.xml
        --gtest_catch_exceptions=0
        DISCOVERY_TIMEOUT 120
        PROPERTIES
        TIMEOUT 120
    )

    target_link_libraries(${cppCoroutine_test_name} cppCoroutine gtest gmock_main)

    # 设置测试目标属性
    set_target_properties(${cppCoroutine_test_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    )

    # 添加测试
    add_test(NAME ${cppCoroutine_test_name} COMMAND ${CMAKE_BINARY_DIR}/test/${cppCoroutine_test_name})
endforeach ()