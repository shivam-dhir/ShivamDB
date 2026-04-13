# CMakeList.txt : CMake project for ShivamDB, include source and define
# project specific logic here.
cmake_minimum_required(VERSION 3.16)

project(ShivamDB)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_library(shivamdb
    src/shivamdb.cpp
 "include/shivamdb/wal.h" "src/wal.cpp")

add_executable(shivamdb_cli
    cli/main.cpp
 "include/shivamdb/wal.h" "src/wal.cpp")

target_link_libraries(shivamdb_cli shivamdb)

