cmake_minimum_required(VERSION 3.24)
add_compile_options(-D_WIN32_WINNT=0x0A00)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(MAIN Edsoa_Example)
project(${MAIN} C CXX)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PROJECT_ROOT})

set(UTIL util)
file(GLOB_RECURSE UTIL_SRC_FILES "${PROJECT_ROOT}/${UTIL}/*.h" "${PROJECT_ROOT}/${UTIL}/*.cc")
add_library(${UTIL} ${UTIL_SRC_FILES})
    find_package(absl CONFIG REQUIRED)
    target_link_libraries(${UTIL} PUBLIC abseil::abseil)

add_executable(${MAIN})
    set(MAIN_SRC ${PROJECT_ROOT}/core/main.cc)
    target_sources(${MAIN} PRIVATE ${MAIN_SRC})
    target_link_libraries(${MAIN} PUBLIC ${UTIL})

file(GLOB CORE_MODULES ${PROJECT_ROOT}/core/*)
list(REMOVE_ITEM CORE_MODULES ${MAIN_SRC})
foreach(CORE_MODULE_PATH ${CORE_MODULES})
    # Get module name
    get_filename_component(MODULE_NAME ${CORE_MODULE_PATH} NAME)
    message(STATUS "[INFO] Found module: ${MODULE_NAME}")
    set(MODULE_DIR "${PROJECT_ROOT}/core/${MODULE_NAME}")

    # Construct module
    file(GLOB_RECURSE SRC_FILES "${MODULE_DIR}/*.h" "${MODULE_DIR}/*.cc")
    add_library(${MODULE_NAME} ${SRC_FILES})
    target_link_libraries(${MODULE_NAME} PUBLIC ${UTIL})

    # Search third-party dependencies and added them to module
    file(GLOB_RECURSE THIRD_PARTY_LIBS ${MODULE_DIR}/*.cmake)
    foreach(THIRD_PARTY_LIB ${THIRD_PARTY_LIBS})
        include(${THIRD_PARTY_LIB})
    endforeach()

    # Add module to MAIN
    target_link_libraries(${MAIN} PUBLIC ${MODULE_NAME})
endforeach()

