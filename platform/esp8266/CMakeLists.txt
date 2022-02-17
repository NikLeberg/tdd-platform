# For how this all comes together see:
# https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html

message(STATUS "Processing platform/esp8266 directory...")

# Check if export.sh was sourced. We need the variables that it sets.
if("$ENV{IDF_PATH}" STREQUAL "")
    message(FATAL_ERROR "No $IDF_PATH variable set! Please run 'source <directory of ESP8266_RTOS_SDK>/export.sh' before starting cmake.")
endif()

# Set toolchain for Cmake.
set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/tools/cmake/toolchain-esp8266.cmake CACHE STRING "Toolchain file")

# Don't build shared libs, no dynamic load & link possible.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "don't build shared libs")

# Set platform options
set(PLATFORM_SUPPORTS_GPIO ON CACHE BOOL "")

# Implement hook that gets run after project() was called.
function(platform_project_init_hook_impl)
    # Add sources to platform abstraction library.
    target_sources(platform PRIVATE
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/platform.c
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gpio.c
    )
    # Fix: Trigger regeneration of sdkconfig after clean.
    set_property(TARGET platform APPEND PROPERTY
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
    idf_build_process(esp8266
        COMPONENTS esp8266 freertos esptool_py pthread
        SDKCONFIG_DEFAULTS ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/sdkconfig.defaults
        SDKCONFIG ${CMAKE_BINARY_DIR}/sdkconfig
        BUILD_DIR ${CMAKE_BINARY_DIR}
    )
endfunction()

# Implement hook that gets run after the executable target was created.
function(platform_main_finish_hook_impl executable_target)
    # Include CMake file that provides ESP-IDF CMake build system APIs.
    include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
    # Link idf components to the platform library.
    target_link_libraries(platform PRIVATE
        idf::esp8266 idf::freertos idf::spi_flash idf::pthread
    )
    # Fix: Final linking of app fails with undefined reference in startup.c.
    # Force inclusion of app_main from platform implementation.
    target_link_options(platform PUBLIC -u app_main)
    # Let the build system know what the project executable is to attach more
    # targets, dependencies, etc.
    idf_build_executable(${executable_target})
    # Fix: Post-Build generation of binary image is started in the topmost build
    # directory but the executable is in /src subfolder. Create symlink.
    execute_process(COMMAND cmake -E create_symlink
        "${CMAKE_BINARY_DIR}/src/${executable_target}"
        "${CMAKE_BINARY_DIR}/${executable_target}"
    )
    # Fix: RTOS_SDK does not seem to define monitor target like esp-idf does.
    find_program(IDF_MONITOR_EXECUTABLE idf_monitor.py REQUIRED)
    add_custom_target(
        monitor
        COMMAND ${IDF_MONITOR_EXECUTABLE} --port $ENV{ESPPORT}
            --baud $ENV{ESPBAUD} ${executable_target}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        VERBATIM
    )
endfunction()

# Fix: there is an error in CMakeLists.txt of the pthread component. Patch it.
find_program(GIT_EXECUTABLE git)
if(GIT_EXECUTABLE)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} apply ${CMAKE_CURRENT_LIST_DIR}/pthread.patch
        WORKING_DIRECTORY $ENV{IDF_PATH}
        ERROR_QUIET # Fails if patch is already applied
    )
endif()
