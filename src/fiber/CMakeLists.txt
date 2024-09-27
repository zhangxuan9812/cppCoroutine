add_library(
  cppCoroutine_fiber
  OBJECT
  fiber.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:cppCoroutine_fiber>
  PARENT_SCOPE)