cmake_minimum_required(VERSION 3.23)
project(MPTetris)
set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
set(SDL2_PATH "SDL2")
set(SDL2_IMAGE_PATH "SDL2_image")
set(SDL2_TTF_PATH "SDL2_ttf")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(MPTetris AssetHelper.h RenderHelper.h TetrisApp.h AppComponent.h AssetHelper.cpp RenderHelper.cpp TetrisApp.cpp AppComponent.cpp main.cpp)
target_link_libraries(MPTetris SDL2::Main SDL2::Image SDL2::TTF)
