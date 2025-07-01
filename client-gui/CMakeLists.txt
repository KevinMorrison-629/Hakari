
set(CLIENT_NAME "client-gui")

# collect all file implementations (src and headers) for subproject
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# Create executable with given src files
add_executable(${CLIENT_NAME}
    main_client.cpp
    ${SOURCES}
)

# Make header files in this directory available to current project.
target_include_directories(${CLIENT_NAME} PRIVATE
    ${HEADERS}
)

# Link server executable against dependencies
target_link_libraries(${CLIENT_NAME} PRIVATE
    core_lib
)