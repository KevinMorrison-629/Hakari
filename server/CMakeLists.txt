
set(SERVER_NAME "server")

# collect all file implementations (src and headers) for subproject
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# Find the packages provided by vcpkg.
find_package(dpp CONFIG REQUIRED)

# Create executable with given src files
add_executable(server
    main_server.cpp
    ${SOURCES}
)

# Make header files in this directory available to current project.
target_include_directories(${SERVER_NAME} PRIVATE
    ${HEADERS}
)

# Link server executable against dependencies
target_link_libraries(${SERVER_NAME} PRIVATE
    core_lib
    dpp::dpp
)