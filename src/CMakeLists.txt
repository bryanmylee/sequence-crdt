# Configure header file to pass CMake settings to source code.
configure_file(config.h.in config.h)

add_subdirectory(lib)

# Add executables.
add_executable(main main.c)

# Link the libraries.
target_link_libraries(main PUBLIC sequence)

