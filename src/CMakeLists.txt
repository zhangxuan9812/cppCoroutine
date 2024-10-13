#add_subdirectory(SegmentTree)
#add_subdirectory(Knapsack)
add_subdirectory(thread)
add_subdirectory(fiber)
add_subdirectory(scheduler)
add_subdirectory(timer)
add_subdirectory(iomanager)
add_library(cppCoroutine STATIC ${ALL_OBJECT_FILES})

set(cppCoroutine_LIBS
        #cppCoroutine_SegmentTree
        #cppCoroutine_Knapsack
        cppCoroutine_thread
        cppCoroutine_fiber
        cppCoroutine_scheduler
        cppCoroutine_timer
        cppCoroutine_ioManager
        )

find_package(Threads REQUIRED)

set(cppCoroutine_THIRDPARTY_LIBS
        #duckdb_pg_query
        fmt
        libfort::fort
        Threads::Threads
        Boost::context
        )

target_link_libraries(
        cppCoroutine
        ${cppCoroutine_LIBS}
        ${cppCoroutine_THIRDPARTY_LIBS})

target_include_directories(
        cppCoroutine PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        #cppCoroutine PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
