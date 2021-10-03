# Based upon the STM32CubeIDE FreeRTOS Template from the BFH at
# https://gitlab.ti.bfh.ch/unterricht/carme-m4/templates/freertos_template and
# converted to VSCode with the help of https://github.com/ObKo/stm32-cmake.

# Set toolchain for Cmake.
set(CMAKE_TOOLCHAIN_FILE $ENV{STM32_CMAKE_PATH}/cmake/stm32_gcc.cmake CACHE STRING "Toolchain file")
set(STM32_TOOLCHAIN_PATH $ENV{STM32_TOOLCHAIN_PATH} CACHE STRING "Toolchain path")

# Don't build shared libs, no dynamic load & link possible.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "don't build shared libs")

# Set platform options
set(PLATFORM_SUPPORTS_TEST_TARGET OFF CACHE BOOL "" FORCE)

# Implement hook that gets run after project() was called.
function(platform_project_init_hook_impl)
    # Add platform implementation / abstraction as library.
    add_library(platform_impl
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/platform.c
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/startup.s
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/stm32f4xx_it.c
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/syscalls.c
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/tiny_printf.c
    )
    target_include_directories(platform_impl PRIVATE
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}
    )
    # Add linker skript so that _Min_Heap_Size can be found.
    # Instead of "flash" there is also a "ram" version available.
    stm32_add_linker_script(platform_impl PUBLIC
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/stm32f4_flash.ld
    )
    # Add prebuilt libraries as targets and link them into the platform.
    carme_m4_import_prebuilt_library(BSP)
    carme_m4_import_prebuilt_library(FreeRTOS)
    carme_m4_import_prebuilt_library(sGUI)
    target_link_libraries(platform_impl INTERFACE BSP FreeRTOS sGUI)
    # ToDo: Prebuilt libs have ben compiled with -mfloat-abi=soft, so no FPU
    # support enabled. Fix this?
    target_compile_options(platform INTERFACE 
        -mcpu=cortex-m4 # -mfpu=fpv4-sp-d16 -mfloat-abi=hard
    )
    target_link_options(platform INTERFACE 
        -mcpu=cortex-m4 # -mfpu=fpv4-sp-d16 -mfloat-abi=hard
    )
    # We need to link in both ways to propagate properties.
    # ToDo: is there a better way?
    target_link_libraries(platform INTERFACE platform_impl)
    target_link_libraries(platform_impl PUBLIC platform)
endfunction()

# Implement hook that gets run after the executable target was created.
function(platform_main_finish_hook_impl executable_target)
    # Generate .bin file that can be flashed.
    stm32_generate_binary_file(${executable_target})
    # Create flash & monitor targets.
    find_program(ST_FLASH_EXECUTABLE st-flash REQUIRED)
    add_custom_target(
        flash
        COMMAND ${ST_FLASH_EXECUTABLE} write ${executable_target}.bin 0x08000000
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        VERBATIM
    )
    add_dependencies(flash ${executable_target})
    add_custom_target(monitor)
endfunction()

# Import a prebuilt library as cmake target.
function(carme_m4_import_prebuilt_library library_name)
    add_library(${library_name} STATIC IMPORTED)
    set_property(TARGET ${library_name} PROPERTY IMPORTED_LOCATION
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Libraries/lib${library_name}.a
    )
    target_link_directories(${library_name} INTERFACE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Libraries)
    set(library_include_dir ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Libraries/${library_name})
    if(EXISTS ${library_include_dir})
        target_include_directories(${library_name} INTERFACE ${library_include_dir})
    endif()
endfunction()
