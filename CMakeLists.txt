# CMakeList.txt : CMake project for ShivamDB, include source and define
# project specific logic here.

cmake_minimum_required(VERSION 3.16)

project(ShivamDB)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_library(shivamdb
    src/shivamdb.cpp
    src/wal.cpp
)

target_include_directories(shivamdb PUBLIC include)

add_executable(shivamdb_cli
    cli/main.cpp
)

target_link_libraries(shivamdb_cli PRIVATE shivamdb)

add_executable(shivamdb_tests
    tests/test_shivamdb.cpp
)

target_link_libraries(shivamdb_tests PRIVATE shivamdb)

add_test(NAME ShivamDBTests COMMAND shivamdb_tests)
