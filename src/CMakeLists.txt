message(STATUS "Processing src directory...")

# Initialise custom platform build system (if any).
platform_main_init_hook()

# Add the many modules of the project as libraries that link themself to "main".
add_subdirectory(mylib)
add_subdirectory(buggy_api)

# Define the main project executable as ${CMAKE_PROJECT_NAME}.
add_executable(${CMAKE_PROJECT_NAME} main.c)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE main mylib buggy_api)

# Finalize custom platform build system (if any).
platform_main_finish_hook(${CMAKE_PROJECT_NAME})
