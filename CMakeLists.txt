cmake_minimum_required(VERSION 3.23)
project(MPTetris)

set(CMAKE_CXX_STANDARD 14)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(MPTetris ${SDL2_LIBRARIES})

add_executable(MPTetris main.cpp)
