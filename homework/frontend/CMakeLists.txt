cmake_minimum_required(VERSION 3.0.0)

# Set C and C++ standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project name and version
project(college-compiler VERSION 0.1.0 LANGUAGES C CXX)

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

# Include the lexer subdirectory and make sure its headers are available
add_subdirectory(lexer)

# Find all source files in the root src/ directory
file(GLOB_RECURSE ROOT_SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")

# Set main file path
set(MAIN_FILE_PATH "${CMAKE_SOURCE_DIR}/main.c")

# Remove main.c from ROOT_SOURCES if it was included
list(REMOVE_ITEM ROOT_SOURCES "${MAIN_FILE_PATH}")

# Set executable name
set(EXECUTABLE "college-compiler")

# Add executable target with the collected source files
add_executable(${EXECUTABLE} ${ROOT_SOURCES} ${MAIN_FILE_PATH})

# Make sure to link the lexer library
target_link_libraries(${EXECUTABLE} lexer_lib)

# Add the lexer include directory specifically for the main executable
target_include_directories(${EXECUTABLE} PRIVATE ${CMAKE_SOURCE_DIR}/lexer/include)

# Compile flags for any build type
set(COMPILE_FLAGS
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

# Add compile flags
target_compile_options(${EXECUTABLE} PRIVATE ${COMPILE_FLAGS})

# Link libraries
target_link_options(${EXECUTABLE} PRIVATE -lm)

# Release specific flags
set(RELEASE_FLAGS
    -D_FORTIFY_SOURCE=2
    -O3
)

# Debug specific flags
set(DEBUG_FLAGS
    -g
    -O0
    -fverbose-asm
    -finstrument-functions
    -fanalyzer
    -fno-inline
)

# Apply flags based on the build type
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(${RELEASE_FLAGS})
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(${DEBUG_FLAGS})
endif()
