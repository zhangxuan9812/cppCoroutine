add_library(
  cppCoroutine_ioManager
  OBJECT
  scheduler.cpp)
  
set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:cppCoroutine_scheduler>
  PARENT_SCOPE)