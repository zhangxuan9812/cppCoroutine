if(EXISTS "/home/eecs211/Desktop/cppCoroutine/build/test/cppCoroutine_scheduler_test[1]_tests.cmake")
  include("/home/eecs211/Desktop/cppCoroutine/build/test/cppCoroutine_scheduler_test[1]_tests.cmake")
else()
  add_test(cppCoroutine_scheduler_test_NOT_BUILT cppCoroutine_scheduler_test_NOT_BUILT)
endif()
