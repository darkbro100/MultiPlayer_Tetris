cmake_minimum_required(VERSION 3.23)
project(MPTetris)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(SDL2_PATH "SDL2-2.26.2\\x86_64-w64-mingw32")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

add_executable(MPTetris main.cpp)
target_link_libraries(MPTetris ${SDL2_LIBRARY})
