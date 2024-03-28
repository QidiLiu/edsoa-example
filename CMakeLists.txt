cmake_minimum_required(VERSION 3.24)

set(MAIN Edsoa_Example)
project(${MAIN} C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PROJECT_ROOT})

set(UTIL util)
add_subdirectory(${PROJECT_ROOT}/core/${UTIL})

add_executable(${MAIN})
    target_sources(${MAIN} PRIVATE ${PROJECT_ROOT}/core/main.cc)
    target_link_libraries(${MAIN} PUBLIC ${UTIL})

    add_subdirectory(${PROJECT_ROOT}/core/ExampleNode)


