set(SERVER_NAME "server")

# collect all file implementations (src and headers) for subproject
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(GLOB_RECURSE HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Find the packages provided by vcpkg.
find_package(bsoncxx CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)
find_package(dpp CONFIG REQUIRED)

# Create executable with given src files
add_executable(server
    ${SOURCES}
)

# Make header files in this directory available to current project.
target_include_directories(${SERVER_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${MONGOCXX_INCLUDE_DIRS}
)

# Link server executable against dependencies
target_link_libraries(${SERVER_NAME} PRIVATE
    common_lib
    $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
    $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
    dpp::dpp
)
