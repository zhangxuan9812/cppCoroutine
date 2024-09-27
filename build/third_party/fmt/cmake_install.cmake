# Install script for directory: /home/eecs211/Desktop/cppCoroutine/third_party/fmt

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/eecs211/Desktop/cppCoroutine/build/lib/libfmtd.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/fmt" TYPE FILE FILES
    "/home/eecs211/Desktop/cppCoroutine/third_party/fmt/include/fmt/args.h"
    "/home/eecs211/Desktop/cppCoroutine/third_party/fmt/include/fmt/chrono.h"
    "/home/eecs211/Desktop/cppCoroutine/third_party/fmt/include/fmt/color.h"
    "/home/eecs211/Desktop/cppCoroutine/third_party/fmt/include/fmt/compile.h"
    "/home/eecs211/Desktop/cppCoroutine/third_party/fmt/include/fmt/core.h"
    "/home/eecs211/Desktop/cppCoroutine/third_party/fmt/include/fmt/format.h"
    "/home/eecs211/Desktop/cppCoroutine/third_party/fmt/include/fmt/format-inl.h"
    "/home/eecs211/Desktop/cppCoroutine/third_party/fmt/include/fmt/os.h"
    "/home/eecs211/Desktop/cppCoroutine/third_party/fmt/include/fmt/ostream.h"
    "/home/eecs211/Desktop/cppCoroutine/third_party/fmt/include/fmt/printf.h"
    "/home/eecs211/Desktop/cppCoroutine/third_party/fmt/include/fmt/ranges.h"
    "/home/eecs211/Desktop/cppCoroutine/third_party/fmt/include/fmt/std.h"
    "/home/eecs211/Desktop/cppCoroutine/third_party/fmt/include/fmt/xchar.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES
    "/home/eecs211/Desktop/cppCoroutine/build/third_party/fmt/fmt-config.cmake"
    "/home/eecs211/Desktop/cppCoroutine/build/third_party/fmt/fmt-config-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets.cmake"
         "/home/eecs211/Desktop/cppCoroutine/build/third_party/fmt/CMakeFiles/Export/lib/cmake/fmt/fmt-targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES "/home/eecs211/Desktop/cppCoroutine/build/third_party/fmt/CMakeFiles/Export/lib/cmake/fmt/fmt-targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES "/home/eecs211/Desktop/cppCoroutine/build/third_party/fmt/CMakeFiles/Export/lib/cmake/fmt/fmt-targets-debug.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/eecs211/Desktop/cppCoroutine/build/third_party/fmt/fmt.pc")
endif()

