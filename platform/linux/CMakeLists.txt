# Check if host is really linux
find_program(CMAKE_EXECUTABLE uname)
exec_program(${CMAKE_EXECUTABLE} OUTPUT_VARIABLE UNAME_OUTPUT)
if(NOT UNAME_OUTPUT STREQUAL "Linux")
    message(FATAL_ERROR "Platform was set to linux but this isn't linux.")
endif()

# Enable supported platform options
set(PLATFORM_SUPPORTS_TEST_TARGET ON CACHE BOOL "")
set(PLATFORM_SUPPORTS_TASKS ON CACHE BOOL "")
set(PLATFORM_SUPPORTS_GPIO ON CACHE BOOL "")

# Implement hook that gets run after project() was called.
function(platform_project_init_hook_impl)
    # Add sources to platform abstraction library.
    target_sources(platform PRIVATE
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/platform.c
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/task.c
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gpio.c
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
    add_custom_target(
        monitor
        COMMAND ${executable_target}
        COMMENT "Executing application..."
    )
endfunction()
