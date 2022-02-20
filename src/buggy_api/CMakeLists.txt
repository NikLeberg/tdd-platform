message(STATUS "Processing src/buggy_api directory...")

add_library(buggy_api buggy_api.c)
target_include_directories(buggy_api PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
