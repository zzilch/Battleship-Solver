cmake_minimum_required(VERSION 3.9)
project(battleship LANGUAGES CXX)

find_package(OpenMP REQUIRED)
add_executable(solver main.cpp)
target_link_libraries(solver PRIVATE OpenMP::OpenMP_CXX)
