# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/eecs211/Desktop/cppCoroutine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/eecs211/Desktop/cppCoroutine/build

# Utility rule file for check-tests.

# Include any custom commands dependencies for this target.
include test/CMakeFiles/check-tests.dir/compiler_depend.make

# Include the progress variables for this target.
include test/CMakeFiles/check-tests.dir/progress.make

test/CMakeFiles/check-tests:
	cd /home/eecs211/Desktop/cppCoroutine/build/test && /usr/bin/ctest --verbose

check-tests: test/CMakeFiles/check-tests
check-tests: test/CMakeFiles/check-tests.dir/build.make
.PHONY : check-tests

# Rule to build all files generated by this target.
test/CMakeFiles/check-tests.dir/build: check-tests
.PHONY : test/CMakeFiles/check-tests.dir/build

test/CMakeFiles/check-tests.dir/clean:
	cd /home/eecs211/Desktop/cppCoroutine/build/test && $(CMAKE_COMMAND) -P CMakeFiles/check-tests.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/check-tests.dir/clean

test/CMakeFiles/check-tests.dir/depend:
	cd /home/eecs211/Desktop/cppCoroutine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/eecs211/Desktop/cppCoroutine /home/eecs211/Desktop/cppCoroutine/test /home/eecs211/Desktop/cppCoroutine/build /home/eecs211/Desktop/cppCoroutine/build/test /home/eecs211/Desktop/cppCoroutine/build/test/CMakeFiles/check-tests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/check-tests.dir/depend

