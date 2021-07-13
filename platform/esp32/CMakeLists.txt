# For how this all comes togethes see:
# https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html

# Set toolchain for Cmake
set(CMAKE_TOOLCHAIN_FILE "$ENV{IDF_PATH}/tools/cmake/toolchain-esp32.cmake" CACHE STRING "Toolchain file")

# Include CMake file that provides ESP-IDF CMake build system APIs.
include("$ENV{IDF_PATH}/tools/cmake/idf.cmake")

# Overwrite add_executable to provide some idf magic
function(add_executable)
    # Run original cmake builtin function
    _add_executable(${ARGV})
    # If this is the main executable then inject custom idf logic
    if(ARG0 STREQUAL CMAKE_PROJECT_NAME)
        # Include ESP-IDF components in the build    
        idf_build_process(esp32)
        # Link newlib of idf
        target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE idf::newlib)
        # Tell idf what the main executable target is
        idf_build_executable(${CMAKE_PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX})
    endif()
endfunction()
