message(STATUS "Processing tests/unit directory...")

add_custom_target(tests_unit)
add_dependencies(tests tests_unit)

# Enable ctest-system
enable_testing()

# Library targets to test. For each target listed, a <name>.c file is expected
# that contains the test code.
set(TEST_LIBS
	mylib
)

# Add target that runs all tests
add_custom_target(
	test_unit_run
	COMMAND ctest --verbose
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Running unit tests..."
	VERBATIM
)
add_dependencies(tests_unit test_unit_run)

# Create test interface library for propagation of common properties.
add_library(test_unit_common INTERFACE)
target_include_directories(test_unit_common INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_compile_options(test_unit_common INTERFACE -fsanitize=undefined,leak,address)
target_link_options(test_unit_common INTERFACE -fsanitize=undefined,leak,address)
target_link_libraries(test_unit_common INTERFACE main cmocka)

# Create test executables for each library unter test.
foreach(TEST_LIB IN ITEMS ${TEST_LIBS})
	add_executable(test_unit_${TEST_LIB} ${TEST_LIB}.c)
	target_link_libraries(test_unit_${TEST_LIB} PRIVATE test_unit_common)
	add_dependencies(test_unit_run test_unit_${TEST_LIBS})
	add_test(NAME test_unit_${TEST_LIB} COMMAND test_unit_${TEST_LIB})
endforeach()
