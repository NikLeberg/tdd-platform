cmake_minimum_required(VERSION 3.13)

# Setup the selected platform (set with -DPLATFORM=xyz on cmake command).
add_subdirectory(platform)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_BUILD_TYPE Debug)

project(esp32 LANGUAGES C)

if (NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
	message(FATAL_ERROR "For now, only GCC is supported.")
endif()

# Initialise custom platform build system (if any).
platform_init_hook()

# Add interface library "main" to propagate dependencies and compiler settings.
add_library(main INTERFACE)
target_compile_features(main INTERFACE c_std_11)
target_compile_options(main INTERFACE
	-W -Wall -Wextra -pedantic -pedantic-errors -Werror
)
target_link_libraries(main INTERFACE platform)

add_subdirectory(lib)
add_subdirectory(src)
# add_subdirectory(test)
add_subdirectory(doc)

# Finalize custom platform build system (if any).
platform_finish_hook()
