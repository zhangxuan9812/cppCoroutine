add_library(
  cppCoroutine_timer
  OBJECT
  timer.cpp)
  
set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:cppCoroutine_timer>
  PARENT_SCOPE)