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

# Include any dependencies generated for this target.
include third_party/linenoise/CMakeFiles/bustub_linenoise.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include third_party/linenoise/CMakeFiles/bustub_linenoise.dir/compiler_depend.make

# Include the progress variables for this target.
include third_party/linenoise/CMakeFiles/bustub_linenoise.dir/progress.make

# Include the compile flags for this target's objects.
include third_party/linenoise/CMakeFiles/bustub_linenoise.dir/flags.make

third_party/linenoise/CMakeFiles/bustub_linenoise.dir/linenoise.c.o: third_party/linenoise/CMakeFiles/bustub_linenoise.dir/flags.make
third_party/linenoise/CMakeFiles/bustub_linenoise.dir/linenoise.c.o: /Users/xuanzhang/Work/cppCoroutine/third_party/linenoise/linenoise.c
third_party/linenoise/CMakeFiles/bustub_linenoise.dir/linenoise.c.o: third_party/linenoise/CMakeFiles/bustub_linenoise.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/xuanzhang/Work/cppCoroutine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object third_party/linenoise/CMakeFiles/bustub_linenoise.dir/linenoise.c.o"
	cd /Users/xuanzhang/Work/cppCoroutine/build/third_party/linenoise && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT third_party/linenoise/CMakeFiles/bustub_linenoise.dir/linenoise.c.o -MF CMakeFiles/bustub_linenoise.dir/linenoise.c.o.d -o CMakeFiles/bustub_linenoise.dir/linenoise.c.o -c /Users/xuanzhang/Work/cppCoroutine/third_party/linenoise/linenoise.c

third_party/linenoise/CMakeFiles/bustub_linenoise.dir/linenoise.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/bustub_linenoise.dir/linenoise.c.i"
	cd /Users/xuanzhang/Work/cppCoroutine/build/third_party/linenoise && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/xuanzhang/Work/cppCoroutine/third_party/linenoise/linenoise.c > CMakeFiles/bustub_linenoise.dir/linenoise.c.i

third_party/linenoise/CMakeFiles/bustub_linenoise.dir/linenoise.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/bustub_linenoise.dir/linenoise.c.s"
	cd /Users/xuanzhang/Work/cppCoroutine/build/third_party/linenoise && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/xuanzhang/Work/cppCoroutine/third_party/linenoise/linenoise.c -o CMakeFiles/bustub_linenoise.dir/linenoise.c.s

# Object files for target bustub_linenoise
bustub_linenoise_OBJECTS = \
"CMakeFiles/bustub_linenoise.dir/linenoise.c.o"

# External object files for target bustub_linenoise
bustub_linenoise_EXTERNAL_OBJECTS =

lib/libbustub_linenoise.a: third_party/linenoise/CMakeFiles/bustub_linenoise.dir/linenoise.c.o
lib/libbustub_linenoise.a: third_party/linenoise/CMakeFiles/bustub_linenoise.dir/build.make
lib/libbustub_linenoise.a: third_party/linenoise/CMakeFiles/bustub_linenoise.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/xuanzhang/Work/cppCoroutine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library ../../lib/libbustub_linenoise.a"
	cd /Users/xuanzhang/Work/cppCoroutine/build/third_party/linenoise && $(CMAKE_COMMAND) -P CMakeFiles/bustub_linenoise.dir/cmake_clean_target.cmake
	cd /Users/xuanzhang/Work/cppCoroutine/build/third_party/linenoise && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bustub_linenoise.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
third_party/linenoise/CMakeFiles/bustub_linenoise.dir/build: lib/libbustub_linenoise.a
.PHONY : third_party/linenoise/CMakeFiles/bustub_linenoise.dir/build

third_party/linenoise/CMakeFiles/bustub_linenoise.dir/clean:
	cd /Users/xuanzhang/Work/cppCoroutine/build/third_party/linenoise && $(CMAKE_COMMAND) -P CMakeFiles/bustub_linenoise.dir/cmake_clean.cmake
.PHONY : third_party/linenoise/CMakeFiles/bustub_linenoise.dir/clean

third_party/linenoise/CMakeFiles/bustub_linenoise.dir/depend:
	cd /Users/xuanzhang/Work/cppCoroutine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/xuanzhang/Work/cppCoroutine /Users/xuanzhang/Work/cppCoroutine/third_party/linenoise /Users/xuanzhang/Work/cppCoroutine/build /Users/xuanzhang/Work/cppCoroutine/build/third_party/linenoise /Users/xuanzhang/Work/cppCoroutine/build/third_party/linenoise/CMakeFiles/bustub_linenoise.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : third_party/linenoise/CMakeFiles/bustub_linenoise.dir/depend
