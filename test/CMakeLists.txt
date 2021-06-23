message(STATUS "Processing test directory...")

# Files with tests
set(TEST_FILES
	mylib.c
)

# Enable ctest-system and create test targets
enable_testing()
set(PROJECT_TEST ${CMAKE_PROJECT_NAME}_test)

find_package(cmocka REQUIRED)

foreach(TEST_FILE IN ITEMS ${TEST_FILES})
	get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
	set(TEST_NAME test_${TEST_NAME})
	list(APPEND TEST_NAMES ${TEST_NAME})
	add_executable(${TEST_NAME} ${TEST_FILE})
	target_link_libraries(${TEST_NAME} PRIVATE mylib)
	target_link_libraries(${TEST_NAME} PRIVATE cmocka)
	add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

# Add running all tests as target
add_custom_target(
    test
    COMMAND ctest --verbose
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running tests..."
    VERBATIM
)
add_dependencies(test ${TEST_NAMES})

# Add cppcheck as target
find_program(CPPCHECK_EXECUTABLE cppcheck)

add_custom_target(
	check
	COMMAND ${CPPCHECK_EXECUTABLE} --project=./compile_commands.json
		--enable=all --inconclusive --suppress=unusedFunction
		--suppress=unmatchedSuppression --suppress=missingIncludeSystem
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build
	COMMENT "Running cppcheck..."
	VERBATIM
)
