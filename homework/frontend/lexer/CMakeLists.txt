# Collect sources and headers from this directory
file(GLOB_RECURSE LEXER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE LEXER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# Add the include directories for this module
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create a library from the sources in this directory
add_library(lexer_lib ${LEXER_SOURCES})

# Optionally add custom compiler flags for this directory
target_compile_options(lexer_lib PRIVATE -Wall -Wextra)

# Ensure the main executable includes the lexer headers
target_include_directories(lexer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
