cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project name and version
project(my_compiler VERSION 0.1.0 LANGUAGES C CXX)

# Include directories for the entire project
include_directories(${CMAKE_SOURCE_DIR})

# Collect sources and includes from the root directory
function(collect_sources_and_includes DIR)
    file(GLOB_RECURSE LOCAL_SOURCES "${DIR}/src/*.c")
    file(GLOB_RECURSE LOCAL_HEADERS "${DIR}/include/*.h")

    # Add include directories
    foreach(HEADER_DIR IN LISTS LOCAL_HEADERS)
        get_filename_component(INCLUDE_DIR ${HEADER_DIR} DIRECTORY)
        include_directories(${INCLUDE_DIR})
    endforeach()

    # Return the collected source files
    set(SOURCES ${LOCAL_SOURCES} PARENT_SCOPE)
endfunction()

# Collect sources from the root directory
collect_sources_and_includes(${CMAKE_SOURCE_DIR})

add_subdirectory(frontend)

# Set main file path
set(MAIN_FILE_PATH "${CMAKE_SOURCE_DIR}/main.c")

# Remove main.c from ROOT_SOURCES if it was included
list(REMOVE_ITEM ROOT_SOURCES "${MAIN_FILE_PATH}")

# Set executable name
set(EXECUTABLE "my_compiler")

# Add executable target with the collected source files
add_executable(${EXECUTABLE} ${ROOT_SOURCES} ${MAIN_FILE_PATH})

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

target_link_libraries(${EXECUTABLE} PRIVATE common_flags lexer_lib parser_lib)

target_include_directories(${EXECUTABLE} PRIVATE
    ${CMAKE_SOURCE_DIR}/lexer/include
    ${CMAKE_SOURCE_DIR}/parser/include
)
