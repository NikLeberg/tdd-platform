message(STATUS "Processing tests/fuzzing directory...")

# Sources for this fuzzing:
# General idea - https://www.code-intelligence.com/fuzzing-with-dependencies
# libFuzzer - https://llvm.org/docs/LibFuzzer.html
# Structure aware - https://github.com/google/fuzzing/blob/master/docs/structure-aware-fuzzing.md
# 
# Ideas for the future:
# Unoptimizing code - https://lafintel.wordpress.com/
# Data flow based - https://releases.llvm.org/3.9.0/docs/LibFuzzer.html#data-flow-guided-fuzzing
#				  - https://github.com/google/oss-fuzz/issues/1632

enable_language(CXX)

add_custom_target(tests_fuzzing)
# The fuzzing target is not built together with the other test targets.

# Include and find definitions for Googles Protocol Buffer.
include(FindProtobuf)
find_package(Protobuf)
find_package(libprotobuf-mutator)
if(NOT Protobuf_FOUND OR NOT libprotobuf-mutator_FOUND)
	# Without those packages fuzzing cant be done.
	return()
endif()

# Create test interface library for propagation of common properties.
add_library(fuzzing_common INTERFACE)
target_include_directories(fuzzing_common INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
	${PROTOBUF_INCLUDE_DIR}
)
target_compile_options(fuzzing_common INTERFACE -fsanitize=fuzzer)
target_link_options(fuzzing_common INTERFACE -fsanitize=fuzzer)
target_link_libraries(fuzzing_common INTERFACE
	platform
	${Protobuf_LIBRARIES}
	libprotobuf-mutator::protobuf-mutator
	libprotobuf-mutator::protobuf-mutator-libfuzzer
)

# Create fuzzing executable for library unter test.
function(fuzzer_add_target lib_name)
	# Duplicate libary test target and instrument it.
	test_target_duplicate(${lib_name} lib_double)
	target_compile_options(${lib_double} PUBLIC -fsanitize=fuzzer,undefined,leak,address)
	target_link_options(${lib_double} PUBLIC -fsanitize=fuzzer,undefined,leak,address)
	# Create a fuzzer executable.
	set(fuzz_exe fuzzing_${lib_name})
	add_executable(${fuzz_exe} ${lib_name}.cc)
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}.proto)
		# If a protobuf file is present, compile and add it.
		protobuf_generate_cpp(proto_src proto_hdr ${lib_name}.proto)
		target_sources(${fuzz_exe} PRIVATE ${proto_src} ${proto_hdr})
		target_include_directories(${fuzz_exe} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
	endif()
	target_link_libraries(${fuzz_exe} PRIVATE fuzzing_common ${lib_double})
	add_dependencies(tests_fuzzing ${fuzz_exe})
	# Create paths for the fuzzing process.
	file(MAKE_DIRECTORY
		${CMAKE_CURRENT_BINARY_DIR}/corpus
		${CMAKE_CURRENT_BINARY_DIR}/merged
	)
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/seeds)
		set(seed_dir ${CMAKE_CURRENT_SOURCE_DIR}/seeds)
	endif()
	# Target to run fuzzer.
	add_custom_target(${fuzz_exe}_run
		COMMAND ./${fuzz_exe}
			-use_value_profile=1 ./corpus ${seed_dir}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		DEPENDS ${fuzz_exe}
		BYPRODUCTS
			${CMAKE_CURRENT_BINARY_DIR}/corpus
	)
	# Target to merge fuzzer corpus.
	add_custom_target(${fuzz_exe}_merge
		COMMAND ./${fuzz_exe}
			-merge=1 ./merged ./corpus
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		DEPENDS ${fuzz_exe}
		BYPRODUCTS
			${CMAKE_CURRENT_BINARY_DIR}/merged
	)
endfunction()

# Add subdirectories of fuzzers for different targets. They are supposed to use
# the fuzzer_add_target function. Each target then is expected to provide a
# <name>.cc file and optionally a <name>.proto file. If a seeds folder exists it
# is also used a seed input for libFuzzer.
add_subdirectory(buggy_api)