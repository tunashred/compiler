# Collect sources and headers from this directory
file(GLOB_RECURSE LEXER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE LEXER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# Create a library from the sources in this directory
add_library(lexer_lib ${LEXER_SOURCES})

# Include the headers for this module
target_include_directories(lexer_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # lexer-specific includes
    ${CMAKE_SOURCE_DIR}/frontend/utils/include  # global utils include
)

# Link common_flags and utils_lib to lexer_lib
target_link_libraries(lexer_lib PUBLIC common_flags utils_lib)
