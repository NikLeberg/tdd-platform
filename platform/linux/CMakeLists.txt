# Check if host is really linux
find_program(CMAKE_EXECUTABLE uname)
exec_program(${CMAKE_EXECUTABLE} OUTPUT_VARIABLE UNAME_OUTPUT)
if(NOT UNAME_OUTPUT STREQUAL "Linux")
    message(FATAL_ERROR "Platform was set to linux but this isn't linux.")
endif()

# Set platform options
set(PLATFORM_SUPPORTS_TEST_TARGET ON)

# Link libraries
target_link_libraries(platform INTERFACE
    m
    pthread
)

# Implement hook that gets run after project() was called.
function(platform_project_init_hook_impl)
    # Add platform implementation / abstraction as library.
    add_library(platform_impl
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/platform.c
    )
    target_link_libraries(platform INTERFACE platform_impl)
    target_link_libraries(platform_impl PUBLIC platform)
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
