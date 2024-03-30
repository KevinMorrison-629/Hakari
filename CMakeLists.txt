set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.0.0)
project(Hakari VERSION 0.1.0 LANGUAGES C CXX)

# Need to build from source (see documentation)

INCLUDE_DIRECTORIES(
    C:/dev_packages/vcpkg/installed/x64-windows/include
)

LINK_DIRECTORIES(
    C:/dev_packages/vcpkg/installed/x64-windows/lib
)

add_executable(${PROJECT_NAME} 
    main.cpp
    HakariORM.cpp
    )

TARGET_LINK_LIBRARIES(${PROJECT_NAME} 
    sqlite3
)