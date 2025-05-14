# 基于Raylib的五子棋小游戏

## 1 开发环境
VsCode

## 2 环境配置
brew install raylib

## 3 运行步骤
1. 编译
clang++ ./src/main.cpp -o gomoku \
    -I/opt/homebrew/include \
    -L/opt/homebrew/lib \
    -lraylib \
    -framework OpenGL \
    -framework Cocoa \
    -std=c++17

2. 运行
./gomoku

## 4 功能介绍

clang++ ./src/test.cpp -o gomoku \
    -I/opt/homebrew/include \
    -L/opt/homebrew/lib \
    -lraylib \
    -framework OpenGL \
    -framework Cocoa \
    -std=c++17