# If pkg-config is not installed on the system, then the CHECK_INSTALL_DIR var
# must be set to the install location of Check.
# set(CHECK_INSTALL_DIR "/usr/local/Cellar/check/0.15.2")

find_package(Check REQUIRED)

include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)

# Since Check uses Threads to parallelize tests, it's mandatory to add pthread
# as a dependency, alongside the Check libraries.
add_executable(main_test main_test.c)
target_link_libraries(main_test ${CHECK_LIBRARIES} pthread sequence)
target_include_directories(main_test PUBLIC ${CHECK_INCLUDE_DIR})

# Create testing target and redirect its output to 'Testing' folder.
add_test(NAME main_test COMMAND main_test WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Testing)
set_tests_properties(main_test PROPERTIES TIMEOUT 30)

# Create check target
add_custom_target(check
    COMMAND $<TARGET_FILE:main_test>
    DEPENDS main_test
)

# Additional Valgrind test to check for memory leaks.
include(FindValgrind)

add_custom_target(valgrind_main_test
    COMMAND ${VALGRIND_PROGRAM} --leak-check=full $<TARGET_FILE:main_test>
    DEPENDS main_test
)

