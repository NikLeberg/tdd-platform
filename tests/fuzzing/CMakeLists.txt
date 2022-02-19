message(STATUS "Processing tests/fuzzing directory...")

# Sources for this fuzzing:
# General idea - https://www.code-intelligence.com/fuzzing-with-dependencies
# libFuzzer - https://llvm.org/docs/LibFuzzer.html
# Structure aware - https://github.com/google/fuzzing/blob/master/docs/structure-aware-fuzzing.md

add_custom_target(tests_fuzzing)
# The fuzzing target is not built together with the other test targets.

# Library targets to fuzz. For each target listed, a <name>.c file is expected
# that contains the fuzzing code.
set(FUZZ_LIBS
	buggy_api
)

# Create test interface library for propagation of common properties.
add_library(fuzzing_common INTERFACE)
target_include_directories(fuzzing_common INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_compile_options(fuzzing_common INTERFACE -fsanitize=fuzzer,undefined,leak,address)
target_link_options(fuzzing_common INTERFACE -fsanitize=fuzzer,undefined,leak,address)
target_link_libraries(fuzzing_common INTERFACE main)

# Create fuzzing executables for each library unter test.
foreach(FUZZ_LIB IN ITEMS ${FUZZ_LIBS})
	set(FUZZ_EXE fuzzing_${FUZZ_LIB})
	add_executable(${FUZZ_EXE} ${FUZZ_LIB}.c)
	target_link_libraries(${FUZZ_EXE} PRIVATE fuzzing_common)
	add_dependencies(tests_fuzzing ${FUZZ_EXE})
	# Create paths for the inputs and outputs during fuzzing.
	file(MAKE_DIRECTORY
		${CMAKE_CURRENT_BINARY_DIR}/${FUZZ_LIB}/results
		${CMAKE_CURRENT_BINARY_DIR}/${FUZZ_LIB}/corpus
		${CMAKE_CURRENT_BINARY_DIR}/${FUZZ_LIB}/seeds
	)
	# Target to run fuzzer
	add_custom_target(${FUZZ_EXE}_run
		COMMAND ../${FUZZ_EXE} -artifact_prefix=./results/ ./corpus ./seeds
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${FUZZ_LIB}
		DEPENDS ${FUZZ_EXE}
	)
endforeach()
