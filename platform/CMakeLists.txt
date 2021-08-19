message(STATUS "Processing platform directory...")

# Interface library to attach all required platform dependencies.
add_library(platform INTERFACE)
target_include_directories(platform INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# List of supported platforms, each platform must have a subfolder.
set(SUPPORTED_PLATFORMS
    "linux"
    "esp32"
    "esp8266"
)

# Options for platforms
option(PLATFORM_SUPPORTS_TEST_TARGET "Platform supports the test-target." ON)

# Run the CMakeLists.txt of the choosen platform.
if(PLATFORM IN_LIST SUPPORTED_PLATFORMS)
    add_subdirectory(${PLATFORM})
else()
    message(FATAL_ERROR "Please set PLATFORM to the platform to use.")
endif()

# Provide a hook function that gets run after project() but bevore any target
# gets defined. Platforms can use this to inject their custom build logic.
function(platform_project_init_hook)
    if(COMMAND platform_project_init_hook_impl)
        platform_project_init_hook_impl()
    endif()
endfunction()

# Provide a hook function that gets run after all CMakeLists.txt were executed.
# Platforms can use this to finalize their build logic.
function(platform_project_finish_hook)
    if(COMMAND platform_project_finish_hook_impl)
        platform_project_finish_hook_impl()
    endif()
endfunction()

# Provide a hook function that gets run bevore the main bulk of the 
# src/CMakeLists.txt gets executed that would create the executable target.
function(platform_main_init_hook)
    if(COMMAND platform_main_init_hook_impl)
        platform_main_init_hook_impl()
    endif()
endfunction()

# Provide a hook function that gets run after the executable target was created.
# Platforms may attach dependencies & co. to this target.
function(platform_main_finish_hook executable_target)
    if(COMMAND platform_main_finish_hook_impl)
        platform_main_finish_hook_impl(${executable_target})
    endif()
endfunction()
