#include "Board.hpp"

// 构造函数初始化颜色
Board::Board() : bgColor{ 210, 180, 140, 255 }, lineColor(BLACK) {}

// 主绘制方法
void Board::Draw() const {
    DrawBackground();
    DrawGridLines();
    DrawStarPoints();
}

// 绘制木质背景
void Board::DrawBackground() const {
    DrawRectangle(
        PADDING - 10, PADDING - 10,
        (SIZE - 1) * CELL_SIZE + 20, (SIZE - 1) * CELL_SIZE + 20,
        bgColor
    );
}

// 绘制网格线
void Board::DrawGridLines() const {
    for (int i = 0; i < SIZE; i++) {
        // 横线
        DrawLineEx(
            Vector2{ (float)(PADDING), (float)(PADDING + i * CELL_SIZE) },
            Vector2{ (float)(PADDING + (SIZE - 1) * CELL_SIZE), (float)(PADDING + i * CELL_SIZE) },
            1.5f,  // 线宽
            lineColor
        );
        // 竖线
        DrawLineEx(
            Vector2{ (float)(PADDING + i * CELL_SIZE), (float)(PADDING) },
            Vector2{ (float)(PADDING + i * CELL_SIZE), (float)(PADDING + (SIZE - 1) * CELL_SIZE) },
            1.5f,
            lineColor
        );
    }
}

// 绘制星位点（天元+四角）
void Board::DrawStarPoints() const {
    const int starPositions[5][2] = { {3,3}, {11,3}, {3,11}, {11,11}, {7,7} }; // 四角 + 天元
    for (const auto& pos : starPositions) {
        DrawCircle(
            PADDING + pos[0] * CELL_SIZE,
            PADDING + pos[1] * CELL_SIZE,
            5,  // 点半径
            lineColor
        );
    }
}