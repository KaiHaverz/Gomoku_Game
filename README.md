# 基于Raylib的五子棋小游戏

## 运行步骤
编译
clang++ ./src/test.cpp -o gomoku \
    -I/opt/homebrew/include \
    -L/opt/homebrew/lib \
    -lraylib \
    -framework OpenGL \
    -framework Cocoa \
    -std=c++17

运行
./gomoku

