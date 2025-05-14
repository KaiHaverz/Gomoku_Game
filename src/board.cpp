// src/board.cpp
#include "board.hpp"
#include "game.hpp"

void InitBoard() {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            board[y][x] = PIECE_EMPTY;
        }
    }
    currentPlayer = PIECE_BLACK;
}

void DrawBoard()
{
    // 1. 绘制棋盘背景
    DrawRectangle(
        PADDING - 10,
        PADDING - 10,
        (BOARD_SIZE - 1) * CELL_SIZE + 20,
        (BOARD_SIZE - 1) * CELL_SIZE + 20,
        BOARD_COLOR);

    // 2. 绘制网格线
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        // 横线
        DrawLineEx(
            Vector2{(float)PADDING, (float)(PADDING + i * CELL_SIZE)},
            Vector2{(float)(PADDING + (BOARD_SIZE - 1) * CELL_SIZE), (float)(PADDING + i * CELL_SIZE)},
            1.5f,
            LINE_COLOR);
        // 竖线
        DrawLineEx(
            Vector2{(float)(PADDING + i * CELL_SIZE), (float)PADDING},
            Vector2{(float)(PADDING + i * CELL_SIZE), (float)(PADDING + (BOARD_SIZE - 1) * CELL_SIZE)},
            1.5f,
            LINE_COLOR);
    }

    // 3. 绘制星位点（天元+四角）
    const int starPoints[5][2] = {{3, 3}, {11, 3}, {3, 11}, {11, 11}, {7, 7}};
    for (const auto &point : starPoints)
    {
        DrawCircle(
            PADDING + point[0] * CELL_SIZE,
            PADDING + point[1] * CELL_SIZE,
            5,
            LINE_COLOR);
    }

    // 4. 绘制棋子
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (board[y][x] == PIECE_BLACK)
            {
                DrawCircle(
                    PADDING + x * CELL_SIZE,
                    PADDING + y * CELL_SIZE,
                    CELL_SIZE / 2 - 2,
                    BLACK);
            }
            else if (board[y][x] == PIECE_WHITE)
            {
                DrawCircle(
                    PADDING + x * CELL_SIZE,
                    PADDING + y * CELL_SIZE,
                    CELL_SIZE / 2 - 2,
                    WHITE);
                DrawCircleLines(
                    PADDING + x * CELL_SIZE,
                    PADDING + y * CELL_SIZE,
                    CELL_SIZE / 2 - 2,
                    BLACK);
            }
        }
    }
}

    // 实现与原来相同...


bool CheckWin(int x, int y)
{
    if (board[y][x] == PIECE_EMPTY)
        return false;

    const int dirs[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    for (const auto &dir : dirs)
    {
        int count = 1;
        int dx = dir[0], dy = dir[1];

        // 正向检查
        for (int i = 1; i < 5; i++)
        {
            int nx = x + dx * i, ny = y + dy * i;
            if (nx >= BOARD_SIZE || ny >= BOARD_SIZE || board[ny][nx] != board[y][x])
                break;
            count++;
        }

        // 反向检查
        for (int i = 1; i < 5; i++)
        {
            int nx = x - dx * i, ny = y - dy * i;
            if (nx < 0 || ny < 0 || board[ny][nx] != board[y][x])
                break;
            count++;
        }

        if (count >= 5)
            return true;
    }
    return false;
}
