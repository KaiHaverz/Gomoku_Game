cmake_minimum_required(VERSION 3.5)  
project(gomoku_oop)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)

# 查找 Raylib
find_package(raylib REQUIRED)

# 添加可执行文件
add_executable(gomoku
    src/main.cpp
    src/Board.cpp
)

# 链接 Raylib 和 macOS 必要框架
target_link_libraries(gomoku raylib)
if(APPLE)
    target_link_libraries(gomoku "-framework OpenGL" "-framework Cocoa")
endif()


include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)