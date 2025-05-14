// src/ui.cpp
#include "ui.hpp"
#include "button.hpp"
#include "game.hpp"
#include "board.hpp"
#include <raylib.h>

//*********************UI设计*********************
void DrawGameUI()
{
    // 当前玩家提示
    const char *text = (currentPlayer == PIECE_BLACK) ? "NOW: BLACK" : "NOW: WHITE";
    DrawText(text, 20, 20, 20, TEXT_COLOR);

    // 胜利提示
    if (gameState == STATE_BLACK_WIN)
    {
        DrawText("BLACK WIN!",
                 WINDOW_WIDTH / 2 - MeasureText("BLACK WIN!", 40) / 2,
                 WINDOW_HEIGHT / 2 - 20,
                 40,
                 RED);
    }
    else if (gameState == STATE_WHITE_WIN)
    {
        DrawText("WHITE WIN!",
                 WINDOW_WIDTH / 2 - MeasureText("WHITE WIN!", 40) / 2,
                 WINDOW_HEIGHT / 2 - 20,
                 40,
                 RED);
    }

    // 操作提示
    DrawText("Press 'R' to return to menu",
             20,
             WINDOW_HEIGHT - 30,
             20,
             DARKGRAY);
}

// 绘制开始菜单
void DrawMenu()
{
    ClearBackground(RAYWHITE);

    // 绘制标题
    DrawText("Gomoku Game",
             WINDOW_WIDTH / 2 - MeasureText("Gomoku Game", 50) / 2,
             150, 50, BLACK);

    // 创建并绘制按钮
    Button btnPair = CreateButton(
        WINDOW_WIDTH / 2 - 100,
        WINDOW_HEIGHT / 2 - 30,
        200, 60, "Pair Game");

    // 更新按钮状态
    btnPair.isHovered = CheckCollisionPointRec(GetMousePosition(), btnPair.bounds);

    // 绘制按钮
    DrawButton(&btnPair);

    // 检测点击
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && btnPair.isHovered)
    {
        gameState = STATE_PLAYING;
        InitBoard();
    }
}
