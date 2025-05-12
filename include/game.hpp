#ifdef GOMOKU_GAME_HPP
#define GOMOKU_GAME_HPP

#include<raylib.h>

// 颜色定义
const Color BOARD_COLOR = {210, 180, 140, 255}; // 木质色
const Color LINE_COLOR = BLACK;
const Color TEXT_COLOR = BLACK;

// 棋子类型
enum Piece
{
    PIECE_EMPTY = 0,
    PIECE_BLACK = 1,
    PIECE_WHITE = 2
};

// 游戏状态
enum GameState
{
    STATE_MENU,      // 菜单
    STATE_PLAYING,   // 游戏进行中
    STATE_BLACK_WIN, // 黑棋胜利
    STATE_WHITE_WIN  // 白棋胜利
};

// 全局变量
Piece board[BOARD_SIZE][BOARD_SIZE]; // 棋盘数组
Piece currentPlayer = PIECE_BLACK;   // 当前玩家
GameState gameState = STATE_MENU;    // 游戏状态


//游戏主循环
void RunGame()



#endif
