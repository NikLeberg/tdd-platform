# For how this all comes together see:
# https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html

# Set toolchain for Cmake.
set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/tools/cmake/toolchain-esp32.cmake CACHE STRING "Toolchain file")

# Implement hook that gets run after project() but bevore the first target is
# created.
function(platform_init_hook_impl)
    # Include CMake file that provides ESP-IDF CMake build system APIs.
    include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
    # Include ESP-IDF components in the build, may be thought as an equivalent
    # of add_subdirectory() but with some additional processing and magic for
    # ESP-IDF build specific build processes.
    idf_build_process(esp32 SDKCONFIG ${CMAKE_BINARY_DIR}/sdkconfig)
endfunction()

# Implement hook that gets run after all CMakeLists.txt were executed.
function(platform_finish_hook_impl)
    # Include CMake file that provides ESP-IDF CMake build system APIs.
    include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
    # Link idf components to the platform interface library.
    target_link_libraries(platform INTERFACE idf::esp32 idf::freertos idf::spi_flash)
    # Let the build system know what the project executable is to attach more
    # targets, dependencies, etc.
    # This assumes that the final project executable name is the project name.
    idf_build_executable(${CMAKE_PROJECT_NAME})
endfunction()
