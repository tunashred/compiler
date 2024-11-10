# Collect sources and headers from this directory
file(GLOB_RECURSE LEXER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE LEXER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# Add the include directories for this module
target_include_directories(lexer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create a library from the sources in this directory
add_library(lexer_lib ${LEXER_SOURCES})

# Link common_flags to lexer_lib
target_link_libraries(lexer_lib PUBLIC common_flags)
