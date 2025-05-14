// include/game.hpp
#ifndef GOMOKU_GAME_HPP
#define GOMOKU_GAME_HPP

#include <raylib.h>

// 游戏常量
const int BOARD_SIZE = 15;
const int CELL_SIZE = 40;
const int PADDING = 50;
const int WINDOW_WIDTH = 850;
const int WINDOW_HEIGHT = 660;

// 颜色定义
const Color BOARD_COLOR = {210, 180, 140, 255};
const Color LINE_COLOR = Color{ 0, 0, 0, 255 };  // 替代BLACK
const Color TEXT_COLOR = Color{ 0, 0, 0, 255 };  // 替代BLACK

// 棋子类型
enum Piece {
    PIECE_EMPTY = 0,
    PIECE_BLACK = 1,
    PIECE_WHITE = 2
};

// 游戏状态
enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_BLACK_WIN,
    STATE_WHITE_WIN
};

// 全局变量声明
extern Piece board[BOARD_SIZE][BOARD_SIZE];
extern Piece currentPlayer;
extern GameState gameState;

// 游戏主循环
void RunGame();

#endif // GOMOKU_GAME_HPP