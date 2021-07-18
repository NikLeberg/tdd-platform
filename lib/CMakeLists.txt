message(STATUS "Processing lib directory...")

# cmocka
# An elegant unit testing framework for C with support for mock objects.
# https://cmocka.org/
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")
set(WITH_EXAMPLES OFF CACHE BOOL "Build examples")
add_subdirectory(cmocka)
# Cmocka's CMakeLists.txt creates symlink of compile_commands.json, remove it.
execute_process(COMMAND cmake -E remove ${CMAKE_SOURCE_DIR}/compile_commands.json)
