message(STATUS "Processing tests/symbolic directory...")

add_custom_target(tests_symbolic)
# The symbolic test targets are not built together with the other test targets.

# Find klee executable its header and its library.
find_program(KLEE_EXECUTABLE klee)
find_path(KLEE_INCLUDE klee.h PATH_SUFFIXES klee)
find_library(KLEE_LIB libkleeRuntest.so)
find_program(KLEE_TEST_EXECUTABLE ktest-tool)
find_program(KLEE_STATS_EXECUTABLE klee-stats)
if(NOT KLEE_EXECUTABLE OR NOT KLEE_INCLUDE OR NOT KLEE_LIB)
	# Without KLEE, symbolic execution cant be done.
	return()
endif()

# Find wllvm to extract bytecode from executable.
find_program(WLLVM_C_EXECUTABLE wllvm)
find_program(WLLVM_CXX_EXECUTABLE wllvm++)
find_program(WLLVM_BC_EXECUTABLE extract-bc)
if(NOT WLLVM_C_EXECUTABLE OR NOT WLLVM_CXX_EXECUTABLE OR NOT WLLVM_BC_EXECUTABLE)
	# Without wllvm, llvm bytecode neccessary for KLEE cant be extracted.
	return()
endif()

# Create interface library for propagation of common properties.
add_library(symbolic_common INTERFACE)
target_include_directories(symbolic_common INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
	${KLEE_INCLUDE}
)
target_compile_options(symbolic_common INTERFACE
	-g # include debug information so KLEE can show function names
	-O0 -Xclang -disable-O0-optnone # enable KLEE to optimize
	-Wno-unused-command-line-argument # ignore warning from wllvm wrapper
)
#target_link_options(symbolic_common INTERFACE -g)
target_link_libraries(symbolic_common INTERFACE
	platform
	${KLEE_LIB}
)

# Create LLVM bytecode for library under test and add KLEE target that runs it.
function(symbolic_add_target lib_name)
	# Duplicate libary test target.
	test_target_duplicate(${lib_name} lib_double)
	target_link_libraries(${lib_double} PRIVATE symbolic_common)
	# Create a executable.
	set(sym_exe symbolic_${lib_name})
	add_executable(${sym_exe} ${lib_name}.c)
	target_link_libraries(${sym_exe} PRIVATE symbolic_common ${lib_double})
	add_dependencies(tests_symbolic ${sym_exe})
	# Set wllvm as compiler wrapper so llvm bytecode can be extracted later.
	set_target_properties(${sym_exe} ${lib_double} PROPERTIES
		C_COMPILER_LAUNCHER "LLVM_COMPILER=clang;${WLLVM_C_EXECUTABLE}"
    	CXX_COMPILER_LAUNCHER "LLVM_COMPILER=clang;${WLLVM_CXX_EXECUTABLE}"
	)
	# Target to execute symbolic test.
	add_custom_target(${sym_exe}_run
		COMMAND ${WLLVM_BC_EXECUTABLE} ${sym_exe}			# extract bytecode
		COMMAND ${KLEE_EXECUTABLE} --entry-point=application_main ${sym_exe}.bc # run KLEE
		COMMAND ${KLEE_TEST_EXECUTABLE} ./klee-last/*.ktest # generated tests
		COMMAND cat ./klee-last/*.err || true 				# found errors
		COMMAND ${KLEE_STATS_EXECUTABLE} ./klee-last 		# print stats
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		DEPENDS ${sym_exe}
		BYPRODUCTS
			${CMAKE_CURRENT_BINARY_DIR}/${sym_exe}.bc
			${CMAKE_CURRENT_BINARY_DIR}/klee-*/
	)
endfunction()

# Add subdirectories of symbolic executors for different targets. They are
# supposed to use the symbolic_add_target function. Each target then is expected
# to provide a <name>.c file.
add_subdirectory(buggy_api)
