# For how this all comes together see:
# https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html

# Check if export.sh was sourced. We need the variables that it sets.
if("$ENV{IDF_PATH}" STREQUAL "")
    message(FATAL_ERROR "No $IDF_PATH variable set! Please run 'source <directory of esp-idf>/export.sh' before starting cmake.")
endif()

# Set toolchain for Cmake.
set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/tools/cmake/toolchain-esp32.cmake CACHE STRING "Toolchain file")

# Implement hook that gets run after project() was called.
function(platform_project_init_hook_impl)
    # Add platform implementation / abstraction as library.
    add_library(platform_impl
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/platform.c
    )
    target_include_directories(platform_impl PUBLIC
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/..
    )
    target_link_libraries(platform INTERFACE platform_impl)
endfunction()

# Implement hook that gets run before ${CMAKE_PROJECT_NAME} is created.
function(platform_main_init_hook_impl)
    # Include CMake file that provides ESP-IDF CMake build system APIs.
    include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
    # Include ESP-IDF components in the build, may be thought as an equivalent
    # of add_subdirectory() but with some additional processing and magic for
    # ESP-IDF build specific build processes.
    idf_build_process(esp32
        COMPONENTS esp32 freertos esptool_py
        SDKCONFIG ${CMAKE_BINARY_DIR}/sdkconfig
        BUILD_DIR ${CMAKE_BINARY_DIR}
    )
endfunction()

# Implement hook that gets run after ${CMAKE_PROJECT_NAME} was created.
function(platform_main_finish_hook_impl)
    # Include CMake file that provides ESP-IDF CMake build system APIs.
    include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
    # Link idf components to the platform interface library.
    target_link_libraries(platform INTERFACE idf::esp32 idf::freertos idf::spi_flash)
    # Let the build system know what the project executable is to attach more
    # targets, dependencies, etc.
    # This assumes that the final project executable name is the project name.
    idf_build_executable(${CMAKE_PROJECT_NAME})
endfunction()
