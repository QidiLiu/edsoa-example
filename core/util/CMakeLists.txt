set(MODULE_NAME util)

find_package(spdlog CONFIG REQUIRED)
find_package(inih CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED)

file(
    GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(${MODULE_NAME} ${SRC_FILES})
    target_link_libraries(${MODULE_NAME} PUBLIC fmt::fmt)
    target_link_libraries(${MODULE_NAME} PUBLIC spdlog::spdlog)
    target_link_libraries(${MODULE_NAME} PUBLIC inih::inih)
    target_link_libraries(${MODULE_NAME} PUBLIC boost::boost)

    target_link_libraries(${MAIN} PUBLIC ${MODULE_NAME})
