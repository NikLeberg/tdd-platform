message(STATUS "Processing platform/linux directory...")

# Check if host is really linux
if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "Platform was set to linux but this isn't linux.")
endif()

# Set toolchain for Cmake.
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/clang.cmake CACHE STRING "Toolchain file")

# Enable supported platform options
set(PLATFORM_SUPPORTS_TEST_TARGET ON CACHE BOOL "")
set(PLATFORM_SUPPORTS_TASKS ON CACHE BOOL "")
set(PLATFORM_SUPPORTS_GPIO ON CACHE BOOL "")
set(PLATFORM_SUPPORTS_PERIPH_I2C ON CACHE BOOL "")
set(PLATFORM_SUPPORTS_SOCKETS ON CACHE BOOL "")

# Implement hook that gets run after project() was called.
function(platform_project_init_hook_impl)
    # Add sources to platform abstraction library.
    target_sources(platform PRIVATE
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/platform.c
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/task.c
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gpio.c
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/i2c.c
    )
    # Link libraries
    target_link_libraries(platform PRIVATE pthread rt)
    target_link_libraries(platform PUBLIC m fff)
endfunction()

# Implement hook that gets run after the executable target was created.
function(platform_main_finish_hook_impl executable_target)
    # Create fake "flash" target. Do nothing.
    add_custom_target(flash)
    # Create "monitor" target. It runs the compiled application.
    get_target_property(executable_dir ${executable_target} BINARY_DIR)
    add_custom_target(
        monitor
        COMMAND ./${executable_target}
        WORKING_DIRECTORY ${executable_dir}
        COMMENT "Executing application..."
    )
    add_dependencies(monitor ${executable_target})
endfunction()
