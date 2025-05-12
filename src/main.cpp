#include <raylib.h>

//*********************一些常量的定义*********************
// 游戏常量
const int BOARD_SIZE = 15;     // 15x15棋盘
const int CELL_SIZE = 40;      // 格子大小
const int PADDING = 50;        // 边缘留白
const int WINDOW_WIDTH = 850;  // 宽
const int WINDOW_HEIGHT = 660; // 高





//*********************按钮*********************
// 按钮结构体
struct Button
{
    Rectangle bounds;
    const char *text;
    bool isHovered;
};

// 初始化按钮
Button CreateButton(float x, float y, float width, float height, const char *text)
{
    return (Button){
        .bounds = {x, y, width, height},
        .text = text,
        .isHovered = false};
}

// 绘制按钮
void DrawButton(Button *button)
{
    Color btnColor = button->isHovered ? SKYBLUE : LIGHTGRAY;
    DrawRectangleRec(button->bounds, btnColor);
    DrawRectangleLinesEx(button->bounds, 2, DARKGRAY);

    int textWidth = MeasureText(button->text, 30);
    DrawText(button->text,
             button->bounds.x + (button->bounds.width - textWidth) / 2,
             button->bounds.y + 15,
             30, DARKGRAY);
}



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

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Gomoku Game");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        if (gameState == STATE_MENU)
        {
            DrawMenu();
        }
        else
        {
            // 游戏逻辑
            if (gameState == STATE_PLAYING)
            {
                Vector2 mousePos = GetMousePosition();

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    int x = (mousePos.x - PADDING) / CELL_SIZE;
                    int y = (mousePos.y - PADDING) / CELL_SIZE;

                    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[y][x] == PIECE_EMPTY)
                    {
                        board[y][x] = currentPlayer;

                        if (CheckWin(x, y))
                        {
                            gameState = (currentPlayer == PIECE_BLACK) ? STATE_BLACK_WIN : STATE_WHITE_WIN;
                        }
                        else
                        {
                            currentPlayer = (currentPlayer == PIECE_BLACK) ? PIECE_WHITE : PIECE_BLACK;
                        }
                    }
                }
            }

            if (IsKeyPressed(KEY_R))
            {
                gameState = STATE_MENU;
            }

            // 绘制游戏
            ClearBackground(RAYWHITE);
            DrawBoard();
            DrawGameUI();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}