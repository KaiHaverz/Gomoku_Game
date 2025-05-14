// src/game.cpp
#include "game.hpp"

// 1. 定义全局变量（在game.hpp中用extern声明）
Piece board[BOARD_SIZE][BOARD_SIZE];  // 棋盘数组
Piece currentPlayer = PIECE_BLACK;    // 当前玩家，初始为黑棋
GameState gameState = STATE_MENU;     // 游戏状态，初始为菜单

// 2. 定义全局函数（如果有）

void RunGame() {
    // 可以在这里实现游戏主循环（如果从main.cpp移出来）
    // 当前示例中，主循环仍在main.cpp，因此此函数可为空
}