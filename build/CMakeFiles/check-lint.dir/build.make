# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.28.3/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.28.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/xuanzhang/Work/cppCoroutine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/xuanzhang/Work/cppCoroutine/build

# Utility rule file for check-lint.

# Include any custom commands dependencies for this target.
include CMakeFiles/check-lint.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/check-lint.dir/progress.make

CMakeFiles/check-lint:
	echo '/Users/xuanzhang/Work/cppCoroutine/include/fiber/fiber.h /Users/xuanzhang/Work/cppCoroutine/include/scheduler/scheduler.h /Users/xuanzhang/Work/cppCoroutine/include/thread/thread.h /Users/xuanzhang/Work/cppCoroutine/src/fiber/fiber.cpp /Users/xuanzhang/Work/cppCoroutine/src/scheduler/scheduler.cpp /Users/xuanzhang/Work/cppCoroutine/src/thread/thread.cpp /Users/xuanzhang/Work/cppCoroutine/test/fiber/cppCoroutine_fiber_test.cpp /Users/xuanzhang/Work/cppCoroutine/test/scheduler/cppCoroutine_scheduler_test.cpp /Users/xuanzhang/Work/cppCoroutine/test/thread/cppCoroutine_thread_test.cpp' | xargs -n12 -P8 CPPLINT_BIN-NOTFOUND --verbose=2 --quiet --linelength=120 --filter=-legal/copyright,-build/header_guard,-runtime/references

check-lint: CMakeFiles/check-lint
check-lint: CMakeFiles/check-lint.dir/build.make
.PHONY : check-lint

# Rule to build all files generated by this target.
CMakeFiles/check-lint.dir/build: check-lint
.PHONY : CMakeFiles/check-lint.dir/build

CMakeFiles/check-lint.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/check-lint.dir/cmake_clean.cmake
.PHONY : CMakeFiles/check-lint.dir/clean

CMakeFiles/check-lint.dir/depend:
	cd /Users/xuanzhang/Work/cppCoroutine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/xuanzhang/Work/cppCoroutine /Users/xuanzhang/Work/cppCoroutine /Users/xuanzhang/Work/cppCoroutine/build /Users/xuanzhang/Work/cppCoroutine/build /Users/xuanzhang/Work/cppCoroutine/build/CMakeFiles/check-lint.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/check-lint.dir/depend

