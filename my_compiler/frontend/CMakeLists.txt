# Include directories for frontend, so subdirectories can access each other's headers
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/frontend/lexer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/frontend/parser/include
    ${CMAKE_CURRENT_SOURCE_DIR}/frontend/utils/include
)

message(STATUS "Dirs inside frontend: ${CMAKE_CURRENT_SOURCE_DIR}")

# Add subdirectories for lexer, parser, and utils
# add_subdirectory(utils)
add_subdirectory(lexer)
add_subdirectory(parser)
