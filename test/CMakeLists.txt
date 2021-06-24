message(STATUS "Processing test directory...")

# Enable builtin cppcheck support
find_program(CPPCHECK_EXECUTABLE cppcheck)
set(CMAKE_C_CPPCHECK ${CPPCHECK_EXECUTABLE})

# Files with tests, library under test is assumed to be named identical.
set(TEST_FILES
	mylib.c
)

# Enable ctest-system and create test targets
enable_testing()
set(PROJECT_TEST ${CMAKE_PROJECT_NAME}_test)

find_package(cmocka REQUIRED)

foreach(TEST_FILE IN ITEMS ${TEST_FILES})
	get_filename_component(TEST_FILE_NO_EXT ${TEST_FILE} NAME_WE)
	set(TEST_NAME test_${TEST_FILE_NO_EXT})
	list(APPEND TEST_NAMES ${TEST_NAME})
	add_executable(${TEST_NAME} ${TEST_FILE})
	target_link_libraries(${TEST_NAME} PRIVATE ${TEST_FILE_NO_EXT})
	target_link_libraries(${TEST_NAME} PRIVATE cmocka)
	add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

# Add target that runs all tests
add_custom_target(
    test
    COMMAND ctest --verbose
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running tests..."
    VERBATIM
)
add_dependencies(test ${TEST_NAMES})
