# Collect sources and headers from this directory
file(GLOB_RECURSE SEMANTIC_ANALYZER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE SEMANTIC_ANALYZER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# Add the include directories for this module
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create a library from the sources in this directory
add_library(domain_lib ${SEMANTIC_ANALYZER_SOURCES})

# Ensure the main executable and other targets include parser headers
target_include_directories(domain_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link common_flags to parser_lib
target_link_libraries(domain_lib PUBLIC common_flags lexer_lib)
