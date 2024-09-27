# CMake generated Testfile for 
# Source directory: /Users/xuanzhang/Work/cppCoroutine/test
# Build directory: /Users/xuanzhang/Work/cppCoroutine/build/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("/Users/xuanzhang/Work/cppCoroutine/build/test/cppCoroutine_fiber_test[1]_include.cmake")
include("/Users/xuanzhang/Work/cppCoroutine/build/test/cppCoroutine_scheduler_test[1]_include.cmake")
include("/Users/xuanzhang/Work/cppCoroutine/build/test/cppCoroutine_thread_test[1]_include.cmake")
add_test(cppCoroutine_fiber_test "/Users/xuanzhang/Work/cppCoroutine/build/test/cppCoroutine_fiber_test")
set_tests_properties(cppCoroutine_fiber_test PROPERTIES  _BACKTRACE_TRIPLES "/Users/xuanzhang/Work/cppCoroutine/test/CMakeLists.txt;43;add_test;/Users/xuanzhang/Work/cppCoroutine/test/CMakeLists.txt;0;")
add_test(cppCoroutine_scheduler_test "/Users/xuanzhang/Work/cppCoroutine/build/test/cppCoroutine_scheduler_test")
set_tests_properties(cppCoroutine_scheduler_test PROPERTIES  _BACKTRACE_TRIPLES "/Users/xuanzhang/Work/cppCoroutine/test/CMakeLists.txt;43;add_test;/Users/xuanzhang/Work/cppCoroutine/test/CMakeLists.txt;0;")
add_test(cppCoroutine_thread_test "/Users/xuanzhang/Work/cppCoroutine/build/test/cppCoroutine_thread_test")
set_tests_properties(cppCoroutine_thread_test PROPERTIES  _BACKTRACE_TRIPLES "/Users/xuanzhang/Work/cppCoroutine/test/CMakeLists.txt;43;add_test;/Users/xuanzhang/Work/cppCoroutine/test/CMakeLists.txt;0;")
