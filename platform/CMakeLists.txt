message(STATUS "Processing platform directory...")

# List of supported platforms, each platform must have a subfolder.
set(SUPPORTED_PLATFORMS
    "linux"
    "esp32"
    "esp8266"
    "carme-m4"
)

# Run the CMakeLists.txt of the choosen platform.
if(PLATFORM IN_LIST SUPPORTED_PLATFORMS)
    add_subdirectory(${PLATFORM})
else()
    message(FATAL_ERROR "Please set PLATFORM to the platform to use.")
endif()

# Options for platforms, per default nothing is supported.
set(PLATFORM_OPTIONS
    TEST_TARGET
    TASKS
    GPIO
    PERIPH_I2C
    PERIPH_UART
PARENT_SCOPE)
foreach(PLATFORM_OPTION IN LISTS PLATFORM_OPTIONS)
    if(NOT DEFINED PLATFORM_SUPPORTS_${PLATFORM_OPTION})
        set(PLATFORM_SUPPORTS_${PLATFORM_OPTION} OFF)
    endif()
endforeach()

# Provide a hook function that gets run after project() but bevore any target
# gets defined. Platforms can use this to inject their custom build logic. We
# also use it to define the platform abstraction library that the platform 
# implementation can use.
function(platform_project_init_hook)
    # Interface library to attach all required platform dependencies.
    add_library(platform)
    target_include_directories(platform PUBLIC ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/..)
    # Turn enabled platform options into preprocessor definitions.
    foreach(PLATFORM_OPTION IN LISTS PLATFORM_OPTIONS)
        if(PLATFORM_SUPPORTS_${PLATFORM_OPTION})
            target_compile_options(platform PUBLIC -DPLATFORM_SUPPORTS_${PLATFORM_OPTION}=1)
        endif()
    endforeach()
    # Run platform implementation hook.
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
