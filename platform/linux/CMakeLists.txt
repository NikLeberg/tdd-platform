# Check if host is really linux
find_program(CMAKE_EXECUTABLE uname)
exec_program(${CMAKE_EXECUTABLE} OUTPUT_VARIABLE UNAME_OUTPUT)
if(NOT UNAME_OUTPUT STREQUAL "Linux")
    message(FATAL_ERROR "Platform was set to linux but this isn't linux.")
endif()

# Set platform options
set(PLATFORM_SUPPORTS_TEST_TARGET ON)

# Link libraries
target_link_libraries(platform INTERFACE
    m
    pthread
)
