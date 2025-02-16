cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME "hakari")

project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional: If you haven't globally integrated vcpkg, make sure to point to the vcpkg toolchain.
# For example, if vcpkg is cloned to ~/vcpkg, add the following line when invoking CMake:
#   -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
#
# You can also check here and error out if it’s missing:
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  message(FATAL_ERROR "Please invoke CMake with -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

# Find the MongoDB C++ driver.
# vcpkg installs the mongo-cxx-driver which provides a CMake config file.
# According to the driver’s documentation, you typically do:
find_package(mongocxx CONFIG REQUIRED)
include_directories(${MONGOCXX_INCLUDE_DIRS})

find_package(dpp CONFIG REQUIRED)

# Add the "include" directory to the include path.
include_directories(${PROJECT_SOURCE_DIR}/include)

# -------------------------------
# Common source files.
file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE HEADERS
    "${CMAKE_SOURCE_DIR}/include/*.h"
)

# Add your executable (assuming you have a src/main.cpp)
add_executable("${PROJECT_NAME}"
    ${SOURCES}
    ${HEADERS}
)
target_link_libraries("${PROJECT_NAME}" PRIVATE
    dpp::dpp
    mongo::bsoncxx_shared
    mongo::mongocxx_shared
)