// src/main.cpp
#include "game.hpp"
#include "board.hpp"
#include "ui.hpp"

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Gomoku Game");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (gameState == STATE_MENU) {
            DrawMenu();
        } else {
            // 游戏逻辑
            if (gameState == STATE_PLAYING) {
                Vector2 mousePos = GetMousePosition();

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    int x = (mousePos.x - PADDING) / CELL_SIZE;
                    int y = (mousePos.y - PADDING) / CELL_SIZE;

                    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[y][x] == PIECE_EMPTY) {
                        board[y][x] = currentPlayer;

                        if (CheckWin(x, y)) {
                            gameState = (currentPlayer == PIECE_BLACK) ? STATE_BLACK_WIN : STATE_WHITE_WIN;
                        } else {
                            currentPlayer = (currentPlayer == PIECE_BLACK) ? PIECE_WHITE : PIECE_BLACK;
                        }
                    }
                }
            }

            if (IsKeyPressed(KEY_R)) {
                gameState = STATE_MENU;
            }

            // 绘制游戏
            ClearBackground(Color{ 245, 245, 245, 255 });  // 替代RAYWHITE
            DrawBoard();
            DrawGameUI();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}