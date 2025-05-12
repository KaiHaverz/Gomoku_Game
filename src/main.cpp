#include "Board.hpp"

int main() {
    // 初始化窗口（根据棋盘尺寸动态计算）
    const int windowWidth = Board::PADDING * 2 + (Board::SIZE - 1) * Board::CELL_SIZE;
    const int windowHeight = Board::PADDING * 2 + (Board::SIZE - 1) * Board::CELL_SIZE;
    InitWindow(windowWidth, windowHeight, "五子棋 - 棋盘绘制");
    SetTargetFPS(60);

    Board board; // 创建棋盘对象

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);  // 清空背景
            board.Draw();              // 绘制棋盘
        EndDrawing();
    }

    CloseWindow();
    return 0;
}