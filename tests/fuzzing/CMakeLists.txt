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
    # ToDo: make optional
endif()

# Find GraphFuzz executable and its library for structure aware fuzzing.
find_program(GFUZZ_EXECUTABLE gfuzz)
find_library(GFUZZ_LIB graphfuzz)
if(NOT GFUZZ_EXECUTABLE OR NOT GFUZZ_LIB)
    # Without GraphFuzz, fuzzing cant be done.
    return()
    # ToDo: make optional
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
    ${GFUZZ_LIB}
)

# Create fuzzing executable for library unter test.
function(fuzzer_add_target lib_name)
    # Duplicate libary test target and instrument it.
    test_target_duplicate(${lib_name} lib_double)
    target_compile_options(${lib_double} PUBLIC -fsanitize=fuzzer,undefined,leak,address)
    target_link_options(${lib_double} PUBLIC -fsanitize=fuzzer,undefined,leak,address)
    # Create a fuzzer executable.
    set(fuzz_exe fuzzing_${lib_name})
    add_executable(${fuzz_exe})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}.proto)
        # If a protobuf file is present, compile and add it.
        protobuf_generate_cpp(proto_src proto_hdr ${lib_name}.proto)
        target_sources(${fuzz_exe} PRIVATE ${proto_src} ${proto_hdr})
        target_include_directories(${fuzz_exe} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/schema.yaml)
        # If a schema.yaml file is present, generate the fuzzer harness and add
        # the files to the fuzzer executable.
        add_custom_command(
            OUTPUT
                ${CMAKE_CURRENT_BINARY_DIR}/fuzz_exec.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/fuzz_write.cpp
            COMMAND ${GFUZZ_EXECUTABLE} gen cpp ${CMAKE_CURRENT_SOURCE_DIR}/schema.yaml .
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/schema.yaml
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating GraphFuzz fuzzer harness and writer for ${lib_name}."
        )
        target_sources(${fuzz_exe} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/fuzz_exec.cpp)
        target_include_directories(${fuzz_exe} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
        # Add writer executable.
        # ToDo: Fix missing application_main in writer executable
        #file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/fuzz_write.cpp "extern \"C\" {\n#include \"fuzzing.h\"\n}")
        #add_executable(${fuzz_exe}_writer ${CMAKE_CURRENT_BINARY_DIR}/fuzz_write.cpp)
        #target_link_libraries(${fuzz_exe}_writer PRIVATE fuzzing_common ${lib_double})
    else()
        # Otherwise just try to add a normal libFuzzer target.
        target_sources(${fuzz_exe} PRIVATE ${lib_name}.cc)
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
# the fuzzer_add_target function. Each target is then created with one of three
# harness types:
# - normal fuzzer:
#       provide <name>.cc with LLVMFuzzerTestOneInput() function
# - protobuf structure aware fuzzer:
#       provide <name>.cc with DEFINE_PROTO_FUZZER() function
#       provide <name>.proto with protocol buffer representation of data
# - GraphFuzz structure aware API fuzzer:
#       provide schema.yaml with API endpoint definitions
# The type of harness is automatically infered from the available files.
# If a seeds folder exists it is used as seed input for libFuzzer.
add_subdirectory(buggy_api)
