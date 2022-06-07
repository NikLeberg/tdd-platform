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
    set(HAVE_PROTOBUF 0)
    message(WARNING "Can't find protobuf!")
else()
    set(HAVE_PROTOBUF 1)
    add_library(fuzzing_protobuf INTERFACE)
    target_include_directories(fuzzing_protobuf INTERFACE ${PROTOBUF_INCLUDE_DIR})
    target_compile_options(fuzzing_protobuf INTERFACE -fsanitize=fuzzer)
    target_link_options(fuzzing_protobuf INTERFACE -fsanitize=fuzzer)
    target_link_libraries(fuzzing_protobuf INTERFACE ${Protobuf_LIBRARIES})
endif()

# Find libprotobuf-mutator for structure aware fuzzing.
find_package(libprotobuf-mutator)
if(NOT HAVE_PROTOBUF OR NOT libprotobuf-mutator_FOUND)
    set(HAVE_PROTOBUF-MUTATOR 0)
    message(WARNING "Can't find libprotobuf-mutator!")
else()
    set(HAVE_PROTOBUF-MUTATOR 1)
    add_library(fuzzing_protobuf-mutator INTERFACE)
    target_link_libraries(fuzzing_protobuf-mutator INTERFACE
        fuzzing_protobuf
        libprotobuf-mutator::protobuf-mutator
        libprotobuf-mutator::protobuf-mutator-libfuzzer
    )
endif()

# Find GraphFuzz executable and its library for structure aware fuzzing.
find_program(GFUZZ_EXECUTABLE gfuzz)
find_library(GFUZZ_LIB graphfuzz)
if(NOT HAVE_PROTOBUF OR NOT GFUZZ_EXECUTABLE OR NOT GFUZZ_LIB)
    set(HAVE_GRAPHFUZZ 0)
    message(WARNING "Can't find GraphFuzz!")
else()
    set(HAVE_GRAPHFUZZ 1)
    add_library(fuzzing_graphfuzz INTERFACE)
    target_link_libraries(fuzzing_graphfuzz INTERFACE
        fuzzing_protobuf
        ${GFUZZ_LIB}
    )
endif()

# Create test interface library for propagation of common properties.
add_library(fuzzing_common INTERFACE)
target_include_directories(fuzzing_common INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
target_compile_options(fuzzing_common INTERFACE -fsanitize=fuzzer)
target_link_options(fuzzing_common INTERFACE -fsanitize=fuzzer)
target_link_libraries(fuzzing_common INTERFACE platform)

# Create fuzzing executable for library unter test.
function(fuzzer_add_target fuzzer_name lib_name)
    # Duplicate library test target and instrument it.
    test_target_duplicate(${lib_name} lib_double)
    target_compile_options(${lib_double} PUBLIC -fsanitize=fuzzer,undefined,leak,address)
    target_link_options(${lib_double} PUBLIC -fsanitize=fuzzer,undefined,leak,address)
    # Create a fuzzer executable.
    add_executable(${fuzzer_name})
    # Determine what type of harness to generate/use.
    if(HAVE_PROTOBUF-MUTATOR AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}.proto)
        # If a protobuf file is present, compile and add it.
        protobuf_generate_cpp(proto_src proto_hdr ${lib_name}.proto)
        target_sources(${fuzzer_name} PRIVATE ${lib_name}.cc ${proto_src} ${proto_hdr})
        target_include_directories(${fuzzer_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
        target_link_libraries(${fuzzer_name} PRIVATE fuzzing_protobuf-mutator)
    elseif(HAVE_GRAPHFUZZ AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/schema.yaml)
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
        target_sources(${fuzzer_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/fuzz_exec.cpp)
        target_include_directories(${fuzzer_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
        target_link_libraries(${fuzzer_name} PRIVATE fuzzing_graphfuzz)
        # Add writer executable.
        add_executable(${fuzzer_name}_writer ${CMAKE_CURRENT_BINARY_DIR}/fuzz_write.cpp)
        target_link_libraries(${fuzzer_name}_writer PRIVATE fuzzing_graphfuzz ${lib_double})
        # Run writer after fuzzing, for this enable additional custom command.
        set(graphfuzz_extra 1)
    else()
        # Otherwise just try to add a normal libFuzzer target.
        target_sources(${fuzzer_name} PRIVATE ${lib_name}.cc)
    endif()
    target_link_libraries(${fuzzer_name} PRIVATE fuzzing_common ${lib_double})
    add_dependencies(tests_fuzzing ${fuzzer_name})
    # Create paths for the fuzzing process.
    file(MAKE_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}/corpus
        ${CMAKE_CURRENT_BINARY_DIR}/merged
    )
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/seeds)
        set(seed_dir ${CMAKE_CURRENT_SOURCE_DIR}/seeds)
    endif()
    # Target to run fuzzer.
    add_custom_target(${fuzzer_name}_run
        COMMAND ./${fuzzer_name}
            -use_value_profile=1 ./corpus ${seed_dir}
            -exact_artifact_path=./crash || true # never fail
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${fuzzer_name}
        BYPRODUCTS
            ${CMAKE_CURRENT_BINARY_DIR}/corpus
    )
    # Run additional step if this is a GraphFuzz harness. Minimizes crash and
    # then converts graph representation to code representation.
    if(graphfuzz_extra)
        add_custom_command(TARGET ${fuzzer_name}_run POST_BUILD
            COMMAND ${GFUZZ_EXECUTABLE} min ./${fuzzer_name} ./crash
            COMMAND ./${fuzzer_name}_writer ./crash.min | tee ./crash.cc
            DEPENDS ${fuzzer_name}_writer
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Minimizing crash and converting graph to code representation."
        )
        add_dependencies(${fuzzer_name}_run ${fuzzer_name}_writer)
    endif()
    # Target to merge fuzzer corpus.
    add_custom_target(${fuzzer_name}_merge
        COMMAND ./${fuzzer_name}
            -merge=1 ./merged ./corpus
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${fuzzer_name}
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
add_subdirectory(buggy_api_graphfuzz)
add_subdirectory(buggy_api_proto)
