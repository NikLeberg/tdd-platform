cmake_minimum_required(VERSION 3.13)

project(esp32 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_SKIP_INSTALL_RULES TRUE)
set(CMAKE_BUILD_TYPE Debug)

if (NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
	message(FATAL_ERROR "For now, only GCC is supported.")
endif()

# Add interface library "main" that holds transitive information like
# dependencies and compiler settings.
add_library(main INTERFACE)
target_compile_features(main INTERFACE c_std_11)
target_compile_options(main INTERFACE
	-W -Wall -Wextra -pedantic -pedantic-errors -Werror
)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(doc)
