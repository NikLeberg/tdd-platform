message(STATUS "Processing tests directory...")

# Provide one target where all further tests can be attached to.
add_custom_target(tests)

# Unittests and fuzzers need to work with their own duplicate of a target.
function(test_target_duplicate target duplicate)
    get_target_property(target_srcs ${target} SOURCES)
    get_target_property(target_src_dir ${target} SOURCE_DIR)
    get_target_property(target_includes ${target} INCLUDE_DIRECTORIES)
    get_target_property(target_libs ${target} LINK_LIBRARIES)
    list(TRANSFORM target_srcs PREPEND ${target_src_dir}/)
    # Use hash of current lists folder as unique suffix for the duplicate.
    string(MD5 suffix ${CMAKE_CURRENT_LIST_DIR})
    string(SUBSTRING ${suffix} 0 6 suffix)
    set(dup_name ${target}_${suffix})
    set(${duplicate} ${dup_name} PARENT_SCOPE)
    add_library(${dup_name} STATIC ${target_srcs})
    target_include_directories(${dup_name} PUBLIC ${target_includes})
    if(target_libs)
        target_link_libraries(${dup_name} PRIVATE ${target_libs})
    endif()
endfunction()

# Add different types of tests.
add_subdirectory(static)
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(fuzzing)
add_subdirectory(symbolic)
