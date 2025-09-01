set(COMMON_NAME "common_lib")

# collect all file implementations (src and headers) for subproject
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(GLOB_RECURSE HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Create library with given src files
add_library(${COMMON_NAME} STATIC
    ${SOURCES}
)

# Make header files in this directory available to other projects.
target_include_directories(${COMMON_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}
)

# Link library against dependencies
target_link_libraries(${COMMON_NAME} PUBLIC
    quickdb
)
