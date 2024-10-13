add_library(
  cppCoroutine_hook
  OBJECT
  fd_manager.cpp
  hook.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:cppCoroutine_hook>
  PARENT_SCOPE)