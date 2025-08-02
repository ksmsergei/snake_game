cmake_minimum_required(VERSION 3.15)

project(snake_game C)

set(CMAKE_C_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(SDL2 REQUIRED)

add_executable(snake WIN32 src/main.c src/snake.c)

target_link_libraries(snake PRIVATE SDL2::SDL2 SDL2::SDL2main)