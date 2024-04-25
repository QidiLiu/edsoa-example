if(NOT absl_FOUND)
    find_package(absl CONFIG REQUIRED)

    if (absl_FOUND)
        target_link_libraries(${MODULE_NAME} PUBLIC abseil::abseil)
    else()
        message("[ERROR] abseil not found!")
    endif()
endif()
