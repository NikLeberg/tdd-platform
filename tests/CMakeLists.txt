message(STATUS "Processing tests directory...")

# Provide one target where all further tests can be attached to.
add_custom_target(tests)

# Add different types of tests.
add_subdirectory(static)
if(PLATFORM_SUPPORTS_TEST_TARGET)
	add_subdirectory(unit)
	add_subdirectory(integration)
	add_subdirectory(fuzzing)
endif()
