message(STATUS "Processing lib directory...")

# Only include / build these external libraries if the target platform supports
# the test target, as these libs are only needed for that target.
if(PLATFORM_SUPPORTS_TEST_TARGET)

    # cmocka
    # An elegant unit testing framework for C with support for mock objects.
    # https://cmocka.org/
    set(WITH_EXAMPLES OFF CACHE BOOL "Build examples")
    add_subdirectory(cmocka EXCLUDE_FROM_ALL)
    # Cmocka's CMakeLists.txt creates symlink of compile_commands.json, remove it.
    execute_process(COMMAND cmake -E remove ${CMAKE_SOURCE_DIR}/compile_commands.json)

endif()
