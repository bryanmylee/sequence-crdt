cmake_minimum_required(VERSION 3.10)

# Project name and version.
project(sequence-crdt VERSION 0.1)

# Specify the C standard.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Configure header file to pass CMake settings to source code.
configure_file(seqconfig.h.in seqconfig.h)

# To make use of the new library, add the sequence directory as an include
# directory so that header files can be found.
add_subdirectory(sequence)
# use of EXTRA_LIBS to collect up any in-use optional libraries to be later
# linked to the executable.
list(APPEND EXTRA_LIBS sequence)
# Used for optional header files.
list(APPEND EXTRA_INCLUDES ${PROJECT_SOURCE_DIR}/sequence)

# Add executables.
add_executable(sequence-crdt main.c)

# Set the target library language.
set_target_properties(sequence PROPERTIES LINKER_LANGUAGE C)

# Link the libraries.
# Extra libs are defined by the list EXTRA_LIBS.
target_link_libraries(sequence-crdt PUBLIC ${EXTRA_LIBS})

# Configured files will be written into the binary tree.
# Therefore, add the directory to the list of paths to search for include files.
# Include directories must be added after the executables are added.
target_include_directories(sequence-crdt PUBLIC
    "${PROJECT_BINARY_DIR}"
    ${EXTRA_INCLUDES}
)

