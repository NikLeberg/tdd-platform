message(STATUS "Processing platform directory...")

# Interface library to attach all required platform dependencies.
add_library(platform INTERFACE)

# List of supported platforms, each platform should have a subfolder.
set(SUPPORTED_PLATFORMS
    "linux"
    "esp32"
)

# Run the CMakeLists.txt of the choosen platform.
if(PLATFORM IN_LIST SUPPORTED_PLATFORMS)
    add_subdirectory(${PLATFORM})
else()
    message(FATAL_ERROR "Please set PLATFORM to the platform to use.")
endif()

# Provide a hook function that gets run after project() but bevore any target
# gets defined. Platforms can use this to inject their custom build logic.
function(platform_init_hook)
    if(COMMAND platform_init_hook_impl)
        platform_init_hook_impl()
    endif()
endfunction()

# Provide a hook function that gets run after all CMakeLists.txt were executed.
# Platforms can use this to finalize their build logic.
function(platform_finish_hook)
    if(COMMAND platform_finish_hook_impl)
        platform_finish_hook_impl()
    endif()
endfunction()
