cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME "hakari")

project(${PROJECT_NAME} VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# You can also check here and error out if it’s missing:
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  message(FATAL_ERROR "Please invoke CMake with -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

# Add Subprojects
add_subdirectory(client)
add_subdirectory(common)
add_subdirectory(server)