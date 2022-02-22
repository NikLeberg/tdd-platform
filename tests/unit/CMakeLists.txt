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
	tests_unit_run
	COMMAND ctest --verbose
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Running unit tests..."
	VERBATIM
)
add_dependencies(tests_unit tests_unit_run)

# Create test interface library for propagation of common properties.
add_library(unit_common INTERFACE)
target_include_directories(unit_common INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_compile_options(unit_common INTERFACE -fsanitize=undefined,leak,address)
target_link_options(unit_common INTERFACE -fsanitize=undefined,leak,address)
target_link_libraries(unit_common INTERFACE main cmocka)

# Create test executables for each library unter test.
foreach(TEST_LIB IN ITEMS ${TEST_LIBS})
	add_executable(unit_${TEST_LIB} ${TEST_LIB}.c)
	target_link_libraries(unit_${TEST_LIB} PRIVATE unit_common ${TEST_LIB})
	add_dependencies(tests_unit_run unit_${TEST_LIBS})
	add_test(NAME unit_${TEST_LIB} COMMAND unit_${TEST_LIB})
endforeach()
