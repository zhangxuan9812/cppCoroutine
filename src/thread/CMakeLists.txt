add_library(
  cppCoroutine_thread
  OBJECT
  thread.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:cppCoroutine_thread>
  PARENT_SCOPE)