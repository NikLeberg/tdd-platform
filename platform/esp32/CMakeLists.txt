# For how this all comes together see:
# https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html

# Check if export.sh was sourced. We need the variables that it sets.
if("$ENV{IDF_PATH}" STREQUAL "")
    message(FATAL_ERROR "No $IDF_PATH variable set! Please run 'source <directory of esp-idf>/export.sh' before starting cmake.")
endif()

# Set toolchain for Cmake.
set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/tools/cmake/toolchain-esp32.cmake CACHE STRING "Toolchain file")

# Don't build shared libs, no dynamic load & link possible.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "don't build shared libs")

# Set platform options
set(PLATFORM_SUPPORTS_TEST_TARGET OFF CACHE BOOL "" FORCE)

# Implement hook that gets run after project() was called.
function(platform_project_init_hook_impl)
    # Add platform implementation / abstraction as library.
    add_library(platform_impl
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/platform.c
    )
    target_link_libraries(platform INTERFACE platform_impl)
    target_link_libraries(platform_impl PUBLIC platform)
    # Fix: Trigger regeneration of sdkconfig after clean.
    set_property(TARGET platform_impl APPEND PROPERTY
        ADDITIONAL_CLEAN_FILES ${CMAKE_BINARY_DIR}/sdkconfig
    )
endfunction()

# Implement hook that gets run before the executable target is created.
function(platform_main_init_hook_impl)
    # Include CMake file that provides ESP-IDF CMake build system APIs.
    include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
    # Include ESP-IDF components in the build, may be thought as an equivalent
    # of add_subdirectory() but with some additional processing and magic for
    # ESP-IDF build specific build processes.
    idf_build_process(esp32
        COMPONENTS esp32 freertos esptool_py
        SDKCONFIG_DEFAULTS ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/sdkconfig.defaults
        SDKCONFIG ${CMAKE_BINARY_DIR}/sdkconfig
        BUILD_DIR ${CMAKE_BINARY_DIR}
    )
endfunction()

# Implement hook that gets run after the executable target was created.
function(platform_main_finish_hook_impl executable_target)
    # Include CMake file that provides ESP-IDF CMake build system APIs.
    include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
    # Link idf components to the platform interface library.
    target_link_libraries(platform INTERFACE idf::esp32 idf::freertos idf::spi_flash)
    # Let the build system know what the project executable is to attach more
    # targets, dependencies, etc.
    idf_build_executable(${executable_target})
endfunction()

# Fix: there is an invalid certificate "EA-CC" in cacrt_all.pem of the mbedtls
# component. Patch it.
find_program(GIT_EXECUTABLE git)
if(GIT_EXECUTABLE)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} apply ${CMAKE_CURRENT_LIST_DIR}/pthread.patch
        WORKING_DIRECTORY $ENV{IDF_PATH}
        ERROR_QUIET # Fails if patch is already applied
    )
endif()
