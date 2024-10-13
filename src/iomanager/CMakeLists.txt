add_library(
  cppCoroutine_ioManager
  OBJECT
  ioscheduler.cpp)
  
set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:cppCoroutine_ioManager>
  PARENT_SCOPE)