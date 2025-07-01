
set(CORE_NAME "core_lib")

# collect all file implementations (src and headers) for subproject
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# Find the packages provided by vcpkg.
find_package(GameNetworkingSockets CONFIG REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)

# Create library with given src files
add_library(${CORE_NAME} STATIC
    ${SOURCES}
)

# Make header files in this directory available to other projects.
target_include_directories(${CORE_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MONGOCXX_INCLUDE_DIRS}
)

# Link library against dependencies
target_link_libraries(${CORE_NAME} PUBLIC
    GameNetworkingSockets::GameNetworkingSockets_s
    $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
    $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
)