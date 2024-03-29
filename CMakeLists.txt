cmake_minimum_required(VERSION 3.13)

# Setup the selected platform (set with -DPLATFORM=xyz on cmake command).
add_subdirectory(platform)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_BUILD_TYPE Debug)
find_program(CCACHE_EXECUTABLE ccache)
if(CCACHE_EXECUTABLE)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE})
endif()

project(tdd-platform LANGUAGES C ASM)

# Initialise custom platform build system (if any).
platform_project_init_hook()

# Add interface library "main" to propagate dependencies and compiler settings.
add_library(main INTERFACE)
target_compile_features(main INTERFACE c_std_11)
target_compile_options(main INTERFACE
    -W -Wall -Wextra -Wpedantic -Werror
)
target_link_libraries(main INTERFACE platform)

add_subdirectory(lib)
add_subdirectory(src)
if(PLATFORM_SUPPORTS_TEST_TARGET)
    add_subdirectory(tests EXCLUDE_FROM_ALL)
endif()
add_subdirectory(doc)

# Finalize custom platform build system (if any).
platform_project_finish_hook()
