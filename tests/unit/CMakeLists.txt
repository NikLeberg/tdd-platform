message(STATUS "Processing tests/unit directory...")

add_custom_target(tests_unit)
add_dependencies(tests tests_unit)

# Enable ctest-system
enable_testing()

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

# Create test executable for library unter test.
function(unit_add_target lib_name)
	# Duplicate libary test target and instrument it.
	test_target_duplicate(${lib_name} lib_double)
	target_compile_options(${lib_double} PUBLIC -fsanitize=undefined,leak,address)
	target_link_options(${lib_double} PUBLIC -fsanitize=undefined,leak,address)
	# Create a test executable.
	set(unit_exe unit_${lib_name})
	add_executable(${unit_exe} ${lib_name}.c)
	target_link_libraries(${unit_exe} PRIVATE unit_common ${lib_double})
	add_dependencies(tests_unit_run ${unit_exe})
	# Add it to ctest.
	add_test(NAME ${unit_exe} COMMAND ${unit_exe})
endfunction()

# Add subdirectories of unittest for different libraries. They are supposed to
# use the unit_add_target function. Each target then is expected to provide a
# <name>.c file with its unit tests.
add_subdirectory(mylib)
