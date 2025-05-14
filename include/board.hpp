// include/board.hpp
#ifndef GOMOKU_BOARD_HPP
#define GOMOKU_BOARD_HPP

#include "game.hpp"

// 初始化棋盘
void InitBoard();

// 绘制棋盘
void DrawBoard();

// 检查五子连珠
bool CheckWin(int x, int y);

#endif // GOMOKU_BOARD_HPP