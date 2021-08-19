message(STATUS "Processing test directory...")

# Provide one target where all further tests can be attached to.
add_custom_target(tests)

# Add target that checks all source code
find_program(CPPCHECK_EXECUTABLE cppcheck REQUIRED)
add_custom_target(
	test_cppcheck
	# COMMAND ${CPPCHECK_EXECUTABLE} --project=compile_commands.json
	COMMAND ${CPPCHECK_EXECUTABLE} cppcheck . -i lib -i build
		--enable=all --inconclusive --suppress=unusedFunction
		--suppress=unmatchedSuppression --suppress=missingIncludeSystem
		--suppress=missingInclude
		--addon=cert --addon=threadsafety
		--error-exitcode=1
    # WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${PLATFORM}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMENT "Running cppcheck..."
	VERBATIM
)
add_dependencies(tests test_cppcheck)

# Only build test executables if the current platform supports it.
if(PLATFORM_SUPPORTS_TEST_TARGET)
	# Files with tests, library under test is assumed to be named identical.
	set(TEST_FILES
		mylib.c
	)

	# Enable ctest-system and create test targets
	enable_testing()
	foreach(TEST_FILE IN ITEMS ${TEST_FILES})
		get_filename_component(TEST_FILE_NO_EXT ${TEST_FILE} NAME_WE)
		set(TEST_NAME test_${TEST_FILE_NO_EXT})
		list(APPEND TEST_NAMES ${TEST_NAME})
		add_executable(${TEST_NAME} ${TEST_FILE})
		target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
		target_link_libraries(${TEST_NAME} PRIVATE main ${TEST_FILE_NO_EXT} cmocka)
		add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
		# Enable USAN, LSAN and ASAN for all cmocka tests
		target_compile_options(${TEST_NAME} PRIVATE -fsanitize=undefined,leak,address)
		target_link_options(${TEST_NAME} PRIVATE -fsanitize=undefined,leak,address)
		# Dont build on "make all"
		set_target_properties(${TEST_NAME} PROPERTIES EXCLUDE_FROM_ALL TRUE)
	endforeach()

	# Add cmocka target that runs all tests
	add_custom_target(
		test_cmocka
		COMMAND ctest --verbose
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Running tests..."
		VERBATIM
	)
	add_dependencies(test_cmocka ${TEST_NAMES})
	add_dependencies(tests test_cmocka)

endif()