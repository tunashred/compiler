# Collect sources from this directory (if you haven't done it already)
file(GLOB_RECURSE UTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# Create a library from the utils sources
add_library(utils_lib ${UTILS_SOURCES})

# Include the utils-specific includes
target_include_directories(utils_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # utils-specific includes
)

# Link common_flags to utils_lib
target_link_libraries(utils_lib PUBLIC common_flags)
