cmake_minimum_required(VERSION 3.23)
project(MPTetris)
set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
set(SDL2_PATH "SDL2")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

message(STATUS "found: ${SDL2_FOUND}")
message(STATUS "found: ${SDL2MAIN_FOUND}")
message(STATUS "include dir: ${SDL2_INCLUDE_DIRS}")
message(STATUS "lib dir: ${SDL2_LIBRARIES}")

add_executable(MPTetris main.cpp)
target_link_libraries(MPTetris SDL2::Main)
