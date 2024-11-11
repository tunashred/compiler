cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project name and version
project(my_compiler VERSION 0.1.0 LANGUAGES C CXX)

# Set main file path
set(MAIN_FILE_PATH "${CMAKE_SOURCE_DIR}/main.c")

# Add subdirectories for frontend components
add_subdirectory(frontend)

# Add executable target with main.c
add_executable(my_compiler ${MAIN_FILE_PATH})

# Create an INTERFACE library for common compile flags
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

# Link libraries to the main executable
target_link_libraries(my_compiler PRIVATE common_flags utils_lib lexer_lib parser_lib)

# Set include directories for the main executable
target_include_directories(my_compiler PRIVATE
    ${CMAKE_SOURCE_DIR}/frontend/lexer/include
    ${CMAKE_SOURCE_DIR}/frontend/parser/include
    ${CMAKE_SOURCE_DIR}/frontend/utils/include
)
