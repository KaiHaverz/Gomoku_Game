#pragma once
#include <raylib.h>

class Board {
public:
    static const int SIZE = 15;       // 15x15 标准棋盘
    static const int CELL_SIZE = 40;  // 每个格子像素大小
    static const int PADDING = 50;    // 棋盘边距

    Board();
    void Draw() const;  // 绘制棋盘

private:
    Color bgColor;      // 棋盘背景色 (木质棕)
    Color lineColor;    // 网格线颜色 (黑色)

    // 内部方法
    void DrawBackground() const;  // 绘制背景
    void DrawGridLines() const;   // 绘制网格线
    void DrawStarPoints() const;  // 绘制星位点
};