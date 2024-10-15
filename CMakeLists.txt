cmake_minimum_required(VERSION 3.21)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Set the project name
# Specify the C++ standard and that it is required
# Add the description of the project
project(cppCoroutine
        VERSION 0.1
        DESCRIPTION "A Coroutine library for C++"
        LANGUAGES C CXX)


# Set the build type to be a debug build if no build type is specified
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES) 
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE "Debug")
endif()


# If Emscripten is being used, add the appropriate flags which means that exceptions are enabled
if(EMSCRIPTEN)
        add_compile_options(-fexceptions)
        add_link_options(-fexceptions)
endif()


# Check if the user is running CMake in the source directory
# If they are, then tell them to run CMake in a separate build directory
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" PATH_TO_CMAKELISTS_TXT)
if(EXISTS "${PATH_TO_CMAKELISTS_TXT}")
        message(FATAL_ERROR "Do not run CMake in the source directory. Please run CMake in a separate build directory. Use 'mkdir build && cd build && cmake ..'")
endif()


# cppCoroutine_BUILD_SUPPORT_DIR is the directory where the build support files are located
# cppCoroutine_CLANG_SEARCH_PATH is the search path for the Clang compiler
# CMAKE_SOURCE_DIR is the directory where the top-level CMakeLists.txt file is located (the root of the project)
set(cppCoroutine_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")
set(cppCoroutine_CLANG_SEARCH_PATH "/usr/local/bin" "/usr/bin" "/usr/local/opt/llvm/bin" "/usr/local/opt/llvm@14/bin"
        "/opt/homebrew/opt/llvm@14/bin/")


# If the user is using Clang, then check if they are using the correct version of Clang
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if(CMAKE_CXX_COMPILER_VERSION MATCHES "^14.")
                message(STATUS "You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
        else()
                message(WARNING "!! We recommend that you use clang-14 for developing cppCoroutine. You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}, a different version.")
        endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        message(STATUS "You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
else()
        #When the user develop the project on macOS, then we should output a warning message.
        #message(WARNING "!! We recommend that you use clang-14 for developing cppCoroutine. You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}, which is not clang.")
        message(STATUS "You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
endif()


# Enable the testing framework
enable_testing()


# Find the clang-format binary
find_program(CLANG_FORMAT_BIN
        NAMES clang-format clang-format-14
        HINTS ${cppCoroutine_CLANG_SEARCH_PATH})
# If the clang-format binary is not found, then print a warning
# Otherwise, print a message saying that the clang-format binary was found
if("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
        message(WARNING "cppCoroutine couldn't find clang-format.")
else()
        message(STATUS "cppCoroutine found clang-format at ${CLANG_FORMAT_BIN}")
endif()


# Find the clang-tidy binary
find_program(CLANG_TIDY_BIN
        NAMES clang-tidy clang-tidy-14
        HINTS ${cppCoroutine_CLANG_SEARCH_PATH})

if("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
        message(WARNING "cppCoroutine couldn't find clang-tidy.")
else()
        set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
        message(STATUS "cppCoroutine found clang-tidy at ${CLANG_TIDY_BIN}")
endif()


# Find the clang-apply-replacements binary
find_program(CLANG_APPLY_REPLACEMENTS_BIN
        NAMES clang-apply-replacements clang-apply-replacements-14
        HINTS ${cppCoroutine_CLANG_SEARCH_PATH})

if("${CLANG_APPLY_REPLACEMENTS_BIN}" STREQUAL "CLANG_APPLY_REPLACEMENTS_BIN-NOTFOUND")
        message(WARNING "cppCoroutine couldn't find clang-apply-replacements.")
else()
        set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
        message(STATUS "cppCoroutine found clang-apply-replacements at ${CLANG_APPLY_REPLACEMENTS_BIN}")
endif()


find_program(CPPLINT_BIN
        NAMES cpplint cpplint.py
        HINTS "${BUSTUB_BUILD_SUPPORT_DIR}")

if("${CPPLINT_BIN}" STREQUAL "CPPLINT_BIN-NOTFOUND")
        message(WARNING "cppCoroutine couldn't find cpplint.")
else()
        message(STATUS "cppCoroutine found cpplint at ${CPPLINT_BIN}")
endif()


# Set the flags for the sanitizers
if(NOT DEFINED cppCoroutine_SANITIZER)
        set(cppCoroutine_SANITIZER address)
endif()


message("Build mode: ${CMAKE_BUILD_TYPE}")
message("${cppCoroutine_SANITIZER} sanitizer will be enabled in debug mode.")


# Set the flags for the compiler
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-unused-parameter -Wno-attributes") # TODO: remove
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fsanitize=${cppCoroutine_SANITIZER} -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# Output the flags for the compiler
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost REQUIRED COMPONENTS context)
# Includes.
set(cppCoroutine_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(cppCoroutine_TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/test/include)
set(cppCoroutine_THIRD_PARTY_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/third_party
        ${PROJECT_SOURCE_DIR}/third_party/fmt/include
        ${PROJECT_SOURCE_DIR}/third_party/libpg_query/include
        ${PROJECT_SOURCE_DIR}/third_party/argparse/include
        ${PROJECT_SOURCE_DIR}/third_party/cpp_random_distributions
        ${PROJECT_SOURCE_DIR}/third_party/backward-cpp/include
)


include_directories(${cppCoroutine_SRC_INCLUDE_DIR} ${cppCoroutine_TEST_INCLUDE_DIR} ${cppCoroutine_THIRD_PARTY_INCLUDE_DIR}, ${Boost_INCLUDE_DIRS})
include_directories(BEFORE src)


# Disable warnings for a target by defining a function
function(disable_target_warnings NAME)
        target_compile_options(${NAME} PRIVATE "-w")
endfunction()


set(CMAKE_MODULE_PATH "${cppCoroutine_BUILD_SUPPORT_DIR}/cmake;${CMAKE_MODULE_PATH}")


add_subdirectory(third_party)
add_subdirectory(src)

message(STATUS "test directory")
add_subdirectory(test)
add_subdirectory(tools)


string(CONCAT cppCoroutine_FORMAT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/src,"
        "${CMAKE_CURRENT_SOURCE_DIR}/include,"
        "${CMAKE_CURRENT_SOURCE_DIR}/test,"
)

# Runs clang format and updates files in place.
add_custom_target(format ${cppCoroutine_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${cppCoroutine_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${cppCoroutine_FORMAT_DIRS}
        --fix
        --quiet
)

# Runs clang format and exits with a non-zero exit code if any files need to be reformatted
add_custom_target(check-format ${cppCoroutine_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${cppCoroutine_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${cppCoroutine_FORMAT_DIRS}
        --quiet
)

# #########################################
# "make check-lint"
# #########################################
file(GLOB_RECURSE cppCoroutine_LINT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
)

# Balancing act: cpplint.py takes a non-trivial time to launch,
# so process 12 files per invocation, while still ensuring parallelism
add_custom_target(check-lint echo '${cppCoroutine_LINT_FILES}' | xargs -n12 -P8
        ${CPPLINT_BIN}
        --verbose=2 --quiet
        --linelength=120
        --filter=-legal/copyright,-build/header_guard,-runtime/references # https://github.com/cpplint/cpplint/issues/148
)

# ##########################################################
# "make check-clang-tidy" target
# ##########################################################
# runs clang-tidy and exits with a non-zero exit code if any errors are found.
# note that clang-tidy automatically looks for a .clang-tidy file in parent directories
add_custom_target(check-clang-tidy
        ${cppCoroutine_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
)
add_custom_target(fix-clang-tidy
        ${cppCoroutine_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        -clang-apply-replacements-binary ${CLANG_APPLY_REPLACEMENTS_BIN} # using our clang-apply-replacements binary
        -fix # apply suggested changes generated by clang-tidy
)
add_custom_target(check-clang-tidy-diff
        ${cppCoroutine_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        -only-diff # only check diff files to master
)
add_custom_target(fix-clang-tidy-diff
        ${cppCoroutine_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        -clang-apply-replacements-binary ${CLANG_APPLY_REPLACEMENTS_BIN} # using our clang-apply-replacements binary
        -fix # apply suggested changes generated by clang-tidy
        -only-diff # only check diff files to master
)


add_dependencies(check-clang-tidy gtest cppCoroutine) 