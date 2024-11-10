# Collect sources and headers from this directory
file(GLOB_RECURSE UTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE UTILS_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# Add the include directories for this module
target_include_directories(utils_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create a library from the sources in this directory
add_library(utils_lib ${UTILS_SOURCES})

# Link common_flags to utils_lib
target_link_libraries(utils_lib PUBLIC common_flags)
