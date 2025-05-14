cmake_minimum_required(VERSION 3.10)
project(gomoku)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)
include_directories(include)

add_executable(gomoku
    src/main.cpp
    src/board.cpp
    src/button.cpp
    src/game.cpp
    src/ui.cpp
)

target_link_libraries(gomoku raylib)