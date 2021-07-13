message(STATUS "Processing platform directory...")

# List of supported platforms, each platform should have a subfolder
set(SUPPORTED_PLATFORMS
    "linux"
    "esp32"
)

if(PLATFORM IN_LIST SUPPORTED_PLATFORMS)
    add_subdirectory(${PLATFORM})
else()
    message(FATAL_ERROR "Please set PLATFORM to the platform to use.")
endif()
