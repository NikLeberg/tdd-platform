message(STATUS "Processing src/mylib directory...")

add_library(mylib mylib.c)
target_include_directories(mylib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
