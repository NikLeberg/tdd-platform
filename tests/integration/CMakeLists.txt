message(STATUS "Processing tests/integration directory...")

add_custom_target(tests_integration)
add_dependencies(tests tests_integration)
