#include <raylib.h>
#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>
#include <climits>

// 游戏常量
const int BOARD_SIZE = 15;
const int CELL_SIZE = 40;
const int PADDING = 50;
const int WINDOW_WIDTH = PADDING * 2 + (BOARD_SIZE - 1) * CELL_SIZE;
const int WINDOW_HEIGHT = PADDING * 2 + (BOARD_SIZE - 1) * CELL_SIZE;

// 颜色
const Color BOARD_COLOR = { 210, 180, 140, 255 };
const Color LINE_COLOR = BLACK;

// 棋子类型
enum Piece { PIECE_EMPTY = 0, PIECE_BLACK = 1, PIECE_WHITE = 2 };

// 游戏状态
enum GameState { STATE_MENU, STATE_PLAYING, STATE_BLACK_WIN, STATE_WHITE_WIN };

// 全局变量
Piece board[BOARD_SIZE][BOARD_SIZE];
Piece currentPlayer = PIECE_BLACK;
GameState gameState = STATE_MENU;
bool vsAI = false;

// ==================== 初始化棋盘 ====================
void InitBoard() {
    for (int y = 0; y < BOARD_SIZE; y++)
        for (int x = 0; x < BOARD_SIZE; x++)
            board[y][x] = PIECE_EMPTY;
    currentPlayer = PIECE_BLACK;
}

// ==================== 胜负判断 ====================
bool CheckWin(int x, int y) {
    const int dirs[4][2] = {{1,0},{0,1},{1,1},{1,-1}};
    Piece target = board[y][x];
    for (auto [dx, dy] : dirs) {
        int count = 1;
        for (int i = 1; i < 5; ++i) {
            int nx = x + dx * i, ny = y + dy * i;
            if (nx < 0 || ny < 0 || nx >= BOARD_SIZE || ny >= BOARD_SIZE) break;
            if (board[ny][nx] == target) count++;
            else break;
        }
        for (int i = 1; i < 5; ++i) {
            int nx = x - dx * i, ny = y - dy * i;
            if (nx < 0 || ny < 0 || nx >= BOARD_SIZE || ny >= BOARD_SIZE) break;
            if (board[ny][nx] == target) count++;
            else break;
        }
        if (count >= 5) return true;
    }
    return false;
}

// ==================== AI评估函数 ====================
int EvaluatePoint(int x, int y, Piece player) {
    if (board[y][x] != PIECE_EMPTY) return 0;
    int score = 0;
    const int dirs[4][2] = {{1,0},{0,1},{1,1},{1,-1}};
    for (auto [dx, dy] : dirs) {
        int count = 1;
        for (int i = 1; i < 5; ++i) {
            int nx = x + dx * i, ny = y + dy * i;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
                board[ny][nx] == player)
                count++;
            else break;
        }
        for (int i = 1; i < 5; ++i) {
            int nx = x - dx * i, ny = y - dy * i;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
                board[ny][nx] == player)
                count++;
            else break;
        }
        score += pow(10, count);
    }
    return score;
}

// 获取所有候选位置（考虑周围有棋子的空位）
std::vector<std::pair<int, int>> GetCandidateMoves() {
    std::vector<std::pair<int, int>> moves;
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            if (board[y][x] != PIECE_EMPTY) continue;
            // 如果周围有邻居就加入候选
            bool hasNeighbor = false;
            for (int dy = -1; dy <= 1 && !hasNeighbor; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && ny >= 0 && nx < BOARD_SIZE && ny < BOARD_SIZE) {
                        if (board[ny][nx] != PIECE_EMPTY) {
                            hasNeighbor = true;
                            break;
                        }
                    }
                }
            }
            if (hasNeighbor)
                moves.emplace_back(x, y);
        }
    }
    return moves;
}

// ==================== Alpha-Beta 剪枝 ====================
int AlphaBeta(int depth, int alpha, int beta, bool maximizing) {
    if (depth == 0)
        return 0;

    Piece me = maximizing ? PIECE_WHITE : PIECE_BLACK;
    Piece opp = maximizing ? PIECE_BLACK : PIECE_WHITE;

    std::vector<std::pair<int, int>> moves = GetCandidateMoves();
    if (moves.empty()) return 0;

    int bestScore = maximizing ? -INT_MAX : INT_MAX;
    for (auto [x, y] : moves) {
        board[y][x] = me;
        if (CheckWin(x, y)) {
            board[y][x] = PIECE_EMPTY;
            return maximizing ? 1000000 : -1000000;
        }

        int score = AlphaBeta(depth - 1, alpha, beta, !maximizing);
        board[y][x] = PIECE_EMPTY;

        if (maximizing) {
            bestScore = std::max(bestScore, score);
            alpha = std::max(alpha, score);
        } else {
            bestScore = std::min(bestScore, score);
            beta = std::min(beta, score);
        }

        if (beta <= alpha) break;
    }
    return bestScore;
}

// ==================== AI落子 ====================
void AIMove() {
    int bestScore = -INT_MAX;
    int bestX = -1, bestY = -1;

    std::vector<std::pair<int, int>> moves = GetCandidateMoves();
    for (auto [x, y] : moves) {
        board[y][x] = PIECE_WHITE;
        int score = EvaluatePoint(x, y, PIECE_WHITE) + 
                    AlphaBeta(2, -INT_MAX, INT_MAX, false);
        board[y][x] = PIECE_EMPTY;

        if (score > bestScore) {
            bestScore = score;
            bestX = x;
            bestY = y;
        }
    }

    if (bestX != -1 && bestY != -1) {
        board[bestY][bestX] = PIECE_WHITE;
        if (CheckWin(bestX, bestY))
            gameState = STATE_WHITE_WIN;
        else
            currentPlayer = PIECE_BLACK;
    }
}

// ==================== 主函数入口 ====================
int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Gomoku AI");
    InitBoard();
    gameState = STATE_PLAYING;
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // 绘制棋盘
        for (int i = 0; i < BOARD_SIZE; i++) {
            DrawLine(PADDING, PADDING + i * CELL_SIZE,
                     PADDING + (BOARD_SIZE - 1) * CELL_SIZE, PADDING + i * CELL_SIZE, LINE_COLOR);
            DrawLine(PADDING + i * CELL_SIZE, PADDING,
                     PADDING + i * CELL_SIZE, PADDING + (BOARD_SIZE - 1) * CELL_SIZE, LINE_COLOR);
        }

        // 绘制棋子
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                if (board[y][x] == PIECE_BLACK)
                    DrawCircle(PADDING + x * CELL_SIZE, PADDING + y * CELL_SIZE, 16, BLACK);
                else if (board[y][x] == PIECE_WHITE)
                    DrawCircle(PADDING + x * CELL_SIZE, PADDING + y * CELL_SIZE, 16, LIGHTGRAY);
            }
        }

        // 鼠标点击下棋
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && gameState == STATE_PLAYING && currentPlayer == PIECE_BLACK) {
            Vector2 mouse = GetMousePosition();
            int x = round((mouse.x - PADDING) / CELL_SIZE);
            int y = round((mouse.y - PADDING) / CELL_SIZE);

            if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[y][x] == PIECE_EMPTY) {
                board[y][x] = PIECE_BLACK;
                if (CheckWin(x, y)) {
                    gameState = STATE_BLACK_WIN;
                } else {
                    currentPlayer = PIECE_WHITE;
                }
            }
        }

        // AI 行动
        if (currentPlayer == PIECE_WHITE && gameState == STATE_PLAYING) {
            AIMove();
        }

        // 胜利信息
        if (gameState == STATE_BLACK_WIN)
            DrawText("Black Wins!", 20, 20, 30, MAROON);
        else if (gameState == STATE_WHITE_WIN)
            DrawText("White Wins!", 20, 20, 30, BLUE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
