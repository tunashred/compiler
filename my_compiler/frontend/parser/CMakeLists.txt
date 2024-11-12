# Collect sources and headers for parser
file(GLOB_RECURSE PARSER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE PARSER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# Create a library for parser
add_library(parser_lib ${PARSER_SOURCES})

# Set include directories for parser
target_include_directories(parser_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR})

# Link common_flags to parser_lib
target_link_libraries(parser_lib PUBLIC common_flags)
