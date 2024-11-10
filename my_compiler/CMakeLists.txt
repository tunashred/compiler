cmake_minimum_required(VERSION 3.0.0)

# Set project name and version
project(my_compiler VERSION 0.1.0 LANGUAGES C CXX)

# Set global C and C++ standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Global include directories
include_directories(${CMAKE_SOURCE_DIR})

# Create an INTERFACE library for common flags
add_library(common_flags INTERFACE)

# Universal compile flags
target_compile_options(common_flags INTERFACE
    -Wall
    -Wextra
    -pthread
    -Wshadow
    -Wcast-align
    -Wwrite-strings
    -Wcast-qual
    -Wconversion
    -Wunreachable-code
    -fstack-usage
)

# Release-specific flags
target_compile_options(common_flags INTERFACE
    "$<$<CONFIG:Release>:-D_FORTIFY_SOURCE=2;-O3>"
)

# Debug-specific flags
target_compile_options(common_flags INTERFACE
    "$<$<CONFIG:Debug>:-g;-O0;-fverbose-asm;-finstrument-functions;-fno-inline>"
)

# Include the frontend subdirectory
add_subdirectory(frontend)

# Set main file path
set(MAIN_FILE_PATH "${CMAKE_SOURCE_DIR}/main.c")

# Set executable name
set(EXECUTABLE "my_compiler")

# Add executable target
add_executable(${EXECUTABLE} ${MAIN_FILE_PATH})

# Link common_flags and other libraries to the main executable
target_link_libraries(${EXECUTABLE} PRIVATE common_flags lexer_lib parser_lib utils_lib)
