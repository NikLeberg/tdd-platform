message(STATUS "Processing tests/static directory...")

add_custom_target(tests_static)
add_dependencies(tests tests_static)

# Create copy of main compile_commands.json with only the relevant project
# sources. Avoids checks on platform and other libraries.
find_program(JQ_EXECUTABLE jq)
if(JQ_EXECUTABLE)
    add_custom_target(
        test_static_filterCompileCommands
        COMMAND ${JQ_EXECUTABLE}
            '[ .[] | select(.file | contains(\"${PROJECT_NAME}/src\")) ]'
            compile_commands.json > ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    )
else()
    message(WARNING
        "jq for filtering 'compile_commands.json' is not installed. "
        "Falling back to no filtering."
    )
    add_custom_target(
        test_static_filterCompileCommands
        COMMAND ${CMAKE_COMMAND} -E copy
            compile_commands.json
            ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    )
endif()

# Add target that checks all source code with Cppcheck
find_program(CPPCHECK_EXECUTABLE cppcheck)
if(CPPCHECK_EXECUTABLE)
    add_custom_target(
        test_static_cppcheck
        COMMAND ${CPPCHECK_EXECUTABLE} --project=compile_commands.json
            --enable=all --inconclusive --suppress=unusedFunction
            --suppress=unmatchedSuppression --suppress=missingIncludeSystem
            --suppress=missingInclude
            --addon=cert --addon=threadsafety
            --error-exitcode=1
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running cppcheck..."
        VERBATIM
    )
    add_dependencies(test_static_cppcheck test_static_filterCompileCommands)
    add_dependencies(tests_static test_static_cppcheck)
else()
    message(WARNING
        "Cppcheck for static code analysis is not installed. "
        "Static check with Cppcheck will not be performed."
    )
endif()

# Add target that checks all source code with Cppcheck
# ToDo: Fail on error
find_program(CLANGTIDY_EXECUTABLE run-clang-tidy)
if(CLANGTIDY_EXECUTABLE)
    add_custom_target(
        test_static_clangTidy
        COMMAND ${CLANGTIDY_EXECUTABLE} -q -header-filter=.
        -checks=*,-llvmlibc*,-llvm-header-guard,-llvm-include-order
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running clang-tidy..."
    )
    add_dependencies(test_static_clangTidy test_static_filterCompileCommands)
    add_dependencies(tests_static test_static_clangTidy)
else()
    message(WARNING
        "Clang-Tidy for static code analysis is not installed. "
        "Static check with Clang-Tidy will not be performed."
    )
endif()
