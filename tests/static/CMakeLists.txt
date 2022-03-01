message(STATUS "Processing tests/static directory...")

add_custom_target(tests_static)
add_dependencies(tests tests_static)

# Create copy of main compile_commands.json with only the relevant project
# sources. Avoids checks on platform and other libraries. Also fix or replace
# some compiler flags that clang does not understand.
find_program(JQ_EXECUTABLE jq)
find_program(SED_EXECUTABLE sed)
if(JQ_EXECUTABLE AND SED_EXECUTABLE)
    add_custom_target(
        static_filterCompileCommands
        COMMAND ${JQ_EXECUTABLE}
            '[ .[] | select(.file | contains(\"${PROJECT_NAME}/src\")) ]'
            ${CMAKE_BINARY_DIR}/compile_commands.json > compile_commands.json
        COMMAND ${SED_EXECUTABLE} -i 's/-mlongcalls/ /g' compile_commands.json
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        BYPRODUCTS compile_commands.json
    )
else()
    message(WARNING
        "jq and/or sed for filtering 'compile_commands.json' are not installed. "
        "Falling back to no filtering."
    )
    add_custom_target(
        static_filterCompileCommands
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/compile_commands.json
            compile_commands.json
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        BYPRODUCTS compile_commands.json
    )
endif()

# Add target that checks all source code with Cppcheck
find_program(CPPCHECK_EXECUTABLE cppcheck)
if(CPPCHECK_EXECUTABLE)
    add_custom_target(
        static_cppcheck
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
    add_dependencies(static_cppcheck static_filterCompileCommands)
    add_dependencies(tests_static static_cppcheck)
else()
    message(WARNING
        "Cppcheck for static code analysis is not installed. "
        "Static check with Cppcheck will not be performed."
    )
endif()

# Add target that checks all source code with Cppcheck
# ToDo: Fail on error
# ToDo: Does not play nice with carme-m4 platform
find_program(CLANGTIDY_EXECUTABLE NAMES run-clang-tidy run-clang-tidy-13)
if(CLANGTIDY_EXECUTABLE AND NOT PLATFORM STREQUAL "carme-m4")
    add_custom_target(
        static_clangTidy
        COMMAND ${CLANGTIDY_EXECUTABLE} -q -header-filter=.
        -checks=*,-llvmlibc*,-llvm-header-guard,-llvm-include-order,-altera*
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running clang-tidy..."
    )
    add_dependencies(static_clangTidy static_filterCompileCommands)
    add_dependencies(tests_static static_clangTidy)
else()
    message(WARNING
        "Clang-Tidy for static code analysis is not installed. "
        "Static check with Clang-Tidy will not be performed."
    )
endif()
