#include <raylib.h>
#include <climits>
#include <algorithm>
#include <vector>
#include <utility>

// 游戏常量
const int BOARD_SIZE = 15;
const int CELL_SIZE = 40;
const int PADDING = 50;
const int WINDOW_WIDTH = PADDING * 2 + (BOARD_SIZE - 1) * CELL_SIZE;
const int WINDOW_HEIGHT = PADDING * 2 + (BOARD_SIZE - 1) * CELL_SIZE;

// 颜色定义
const Color BOARD_COLOR = { 210, 180, 140, 255 };
const Color LINE_COLOR = BLACK;
const Color TEXT_COLOR = BLACK;

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

// 棋型枚举 
enum PatternType {
    FIVE = 1000000,
    FOUR = 100000,   // 活四
    S_FOUR = 10000,  // 冲四
    THREE = 1000,    // 活三
    S_THREE = 100,   // 眠三
    TWO = 10,        // 活二
    S_TWO = 5        // 眠二
};

// 全局变量
Piece board[BOARD_SIZE][BOARD_SIZE];
Piece currentPlayer = PIECE_BLACK;
GameState gameState = STATE_MENU;
bool vsAI = false;

// AI参数
const int WIN_SCORE = 1000000;
const int MAX_INITIAL_DEPTH = 4;  // 初始最大深度
Vector2 historyTable[BOARD_SIZE][BOARD_SIZE] = {0};  // 历史表

// 前向声明
bool CheckWin(int x, int y);  // 解决未声明错误的关键声明
void InitBoard();

// 按钮结构体
struct Button {
    Rectangle bounds;
    const char* text;
    bool isHovered;
};

// 创建按钮
Button CreateButton(float x, float y, float width, float height, const char* text) {
    return Button{ {x, y, width, height}, text, false };
}

// 绘制按钮
void DrawButton(Button* button) {
    Color btnColor = button->isHovered ? SKYBLUE : LIGHTGRAY;
    DrawRectangleRec(button->bounds, btnColor);
    DrawRectangleLinesEx(button->bounds, 2, DARKGRAY);
    int textWidth = MeasureText(button->text, 30);
    DrawText(button->text, 
            button->bounds.x + (button->bounds.width - textWidth)/2,
            button->bounds.y + 15, 
            30, DARKGRAY);
}

// 初始化棋盘
void InitBoard() {
    for (int y = 0; y < BOARD_SIZE; y++)
        for (int x = 0; x < BOARD_SIZE; x++)
            board[y][x] = PIECE_EMPTY;
    currentPlayer = PIECE_BLACK;
    // 重置历史表
    for(int i=0; i<BOARD_SIZE; i++)
        for(int j=0; j<BOARD_SIZE; j++)
            historyTable[i][j] = {-1,-1};
}

// 棋型评估 
int EvaluatePattern(int x, int y, Piece player) {
    int total = 0;
    const int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
    
    for (auto [dx, dy] : dirs) {
        int consecutive = 1, emptyLeft = 0, emptyRight = 0, blocked = 0;
        
        // 正向扩展
        for (int i = 1; i < 6; i++) {
            int nx = x + dx*i, ny = y + dy*i;
            if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE) {
                blocked++; break;
            }
            if (board[ny][nx] == player) consecutive++;
            else if (board[ny][nx] == PIECE_EMPTY && emptyLeft == 0) {
                emptyLeft = i;
            }
            else {
                blocked++; break;
            }
        }
        
        // 反向扩展
        for (int i = 1; i < 6; i++) {
            int nx = x - dx*i, ny = y - dy*i;
            if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE) {
                blocked++; break;
            }
            if (board[ny][nx] == player) consecutive++;
            else if (board[ny][nx] == PIECE_EMPTY && emptyRight == 0) {
                emptyRight = i;
            }
            else {
                blocked++; break;
            }
        }

        // 棋型分类
        if (consecutive >= 5) return WIN_SCORE;
        if (blocked == 0) {
            if (consecutive == 4) total += FOUR;
            else if (consecutive == 3) total += THREE;
            else if (consecutive == 2) total += TWO;
        }
        else if (blocked == 1) {
            if (consecutive == 4) total += S_FOUR;
            else if (consecutive == 3) total += S_THREE;
            else if (consecutive == 2) total += S_TWO;
        }
        
        // 连续性加分
        if (emptyLeft == 0 && emptyRight == 0) continue;
        total += consecutive * 10;
    }
    return total;
}

// 评估函数 
int EvaluatePosition(int x, int y, Piece player) {
    int positionScore = 0;
    // 中心优先策略
    int centerDist = std::abs(7 - x) + std::abs(7 - y);
    positionScore += (14 - centerDist) * 10;  // 中心区域权重
    
    // 棋型评估
    positionScore += EvaluatePattern(x, y, player);
    
    // 阻断对手评估
    Piece opponent = (player == PIECE_BLACK) ? PIECE_WHITE : PIECE_BLACK;
    positionScore += EvaluatePattern(x, y, opponent) / 2;
    
    return positionScore;
}

// 自定义比较函数解决Vector2比较问题
bool CompareCandidates(const std::pair<int, Vector2>& a, 
                      const std::pair<int, Vector2>& b) {
    return a.first > b.first;  // 降序排列
}

// Alpha-Beta搜索 
std::pair<int, Vector2> AlphaBeta(int depth, int alpha, int beta, bool maximizing, int maxDepth = MAX_INITIAL_DEPTH) {
    if (depth == 0) {
        int maxScore = -INT_MAX;
        Vector2 best = { -1, -1 };
        
        // 移动排序优化
        std::vector<std::pair<int, Vector2>> candidates;
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                if (board[y][x] != PIECE_EMPTY) continue;
                int score = EvaluatePosition(x, y, PIECE_WHITE) - 
                           EvaluatePosition(x, y, PIECE_BLACK);
                candidates.emplace_back(score, Vector2{(float)x, (float)y});
            }
        }
        // 使用自定义比较函数排序
        std::sort(candidates.begin(), candidates.end(), CompareCandidates);
        
        // 评估前N个候选点
        for (int i = 0; i < std::min(20, (int)candidates.size()); i++) {
            int x = candidates[i].second.x;
            int y = candidates[i].second.y;
            if (board[y][x] != PIECE_EMPTY) continue;
            
            int score = candidates[i].first;
            if (score > maxScore) {
                maxScore = score;
                best = { (float)x, (float)y };
            }
        }
        return { maxScore, best };
    }

    Vector2 bestPos = { -1, -1 };
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x] != PIECE_EMPTY) continue;
            
            // 历史表启发 
            if (historyTable[y][x].x != -1 || historyTable[y][x].y != -1) {
                board[y][x] = maximizing ? PIECE_WHITE : PIECE_BLACK;
                auto [score, pos] = AlphaBeta(depth-1, -beta, -alpha, !maximizing, maxDepth);
                score = -score;
                board[y][x] = PIECE_EMPTY;
                
                if (score >= beta) {
                    historyTable[y][x] = { (float)x, (float)y };
                    return { beta, { (float)x, (float)y } };
                }
                if (score > alpha) {
                    alpha = score;
                    bestPos = { (float)x, (float)y };
                }
                continue;
            }
            
            // 普通节点处理
            board[y][x] = maximizing ? PIECE_WHITE : PIECE_BLACK;
            auto [score, pos] = AlphaBeta(depth-1, -beta, -alpha, !maximizing, maxDepth);
            score = -score;
            board[y][x] = PIECE_EMPTY;
            
            if (score >= beta) {
                historyTable[y][x] = { (float)x, (float)y };
                return { beta, { (float)x, (float)y } };
            }
            if (score > alpha) {
                alpha = score;
                bestPos = { (float)x, (float)y };
            }
        }
    }
    return { alpha, bestPos };
}

// 动态深度调整 
void DynamicAIDepth(int& depth) {
    int movesMade = 0;
    for (int y = 0; y < BOARD_SIZE; y++)
        for (int x = 0; x < BOARD_SIZE; x++)
            if (board[y][x] != PIECE_EMPTY) movesMade++;
                
    if (movesMade < 10) depth = 5;  // 开局阶段加深搜索
    else if (movesMade < 20) depth = 4;
    else depth = MAX_INITIAL_DEPTH;
}

// AI走棋 
void AIMove() {
    if (currentPlayer != PIECE_WHITE) return;
    
    // 动态调整搜索深度
    int searchDepth = MAX_INITIAL_DEPTH;
    DynamicAIDepth(searchDepth);
    
    auto [score, pos] = AlphaBeta(searchDepth, -INT_MAX, INT_MAX, true);
    if (pos.x != -1) {
        int x = pos.x, y = pos.y;
        board[y][x] = PIECE_WHITE;
        if (CheckWin(x, y)) {
            gameState = STATE_WHITE_WIN;
        } else {
            currentPlayer = PIECE_BLACK;
        }
    }
}

// 胜负判断
bool CheckWin(int x, int y) {
    if (board[y][x] == PIECE_EMPTY) return false;
    const int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
    for (auto [dx, dy] : dirs) {
        int count = 1;
        // 正向检查
        for (int i = 1; i < 5; i++) {
            int nx = x + dx*i, ny = y + dy*i;
            if (nx >= BOARD_SIZE || ny >= BOARD_SIZE || board[ny][nx] != board[y][x]) break;
            count++;
        }
        // 反向检查
        for (int i = 1; i < 5; i++) {
            int nx = x - dx*i, ny = y - dy*i;
            if (nx < 0 || ny < 0 || board[ny][nx] != board[y][x]) break;
            count++;
        }
        if (count >= 5) return true;
    }
    return false;
}

// 绘制棋盘
void DrawBoard() {
    // 棋盘背景
    DrawRectangle(PADDING-10, PADDING-10, 
                 (BOARD_SIZE-1)*CELL_SIZE+20, (BOARD_SIZE-1)*CELL_SIZE+20, BOARD_COLOR);
    // 网格线
    for (int i = 0; i < BOARD_SIZE; i++) {
        Vector2 start = { (float)PADDING, (float)(PADDING + i*CELL_SIZE) };
        Vector2 end = { (float)(PADDING + (BOARD_SIZE-1)*CELL_SIZE), start.y };
        DrawLineEx(start, end, 1.5f, LINE_COLOR);
        start = { (float)(PADDING + i*CELL_SIZE), (float)PADDING };
        end = { start.x, (float)(PADDING + (BOARD_SIZE-1)*CELL_SIZE) };
        DrawLineEx(start, end, 1.5f, LINE_COLOR);
    }
    // 星位点
    const int stars[5][2] = {{3,3}, {11,3}, {3,11}, {11,11}, {7,7}};
    for (auto [x,y] : stars)
        DrawCircle(PADDING+x*CELL_SIZE, PADDING+y*CELL_SIZE, 5, LINE_COLOR);
    // 棋子
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x] == PIECE_BLACK) {
                DrawCircle(PADDING+x*CELL_SIZE, PADDING+y*CELL_SIZE, CELL_SIZE/2-2, BLACK);
            }
            else if (board[y][x] == PIECE_WHITE) {
                DrawCircle(PADDING+x*CELL_SIZE, PADDING+y*CELL_SIZE, CELL_SIZE/2-2, WHITE);
                DrawCircleLines(PADDING+x*CELL_SIZE, PADDING+y*CELL_SIZE, CELL_SIZE/2-2, BLACK);
            }
        }
    }
}

// 绘制游戏UI
void DrawGameUI() {
    // 当前玩家
    const char* text = (currentPlayer == PIECE_BLACK) ? "Player's Turn" : "AI's Turn";
    DrawText(text, 20, 20, 20, TEXT_COLOR);
    // 胜利提示
    if (gameState == STATE_BLACK_WIN) {
        DrawText("Player Wins!", WINDOW_WIDTH/2-80, WINDOW_HEIGHT/2-20, 40, RED);
    }
    else if (gameState == STATE_WHITE_WIN) {
        DrawText("AI Wins!", WINDOW_WIDTH/2-60, WINDOW_HEIGHT/2-20, 40, RED);
    }
    // 操作提示
    DrawText("Press R to Return", 20, WINDOW_HEIGHT-30, 20, DARKGRAY);
}

// 绘制菜单
void DrawMenu() {
    ClearBackground(RAYWHITE);
    // 标题
    DrawText("Gomoku", WINDOW_WIDTH/2-MeasureText("Gomoku",50)/2, 100, 50, BLACK);
    // 按钮
    Button btnPVP = CreateButton(WINDOW_WIDTH/2-100, 250, 200, 60, "Player vs Player");
    Button btnAI = CreateButton(WINDOW_WIDTH/2-100, 350, 200, 60, "Player vs AI");
    btnPVP.isHovered = CheckCollisionPointRec(GetMousePosition(), btnPVP.bounds);
    btnAI.isHovered = CheckCollisionPointRec(GetMousePosition(), btnAI.bounds);
    DrawButton(&btnPVP);
    DrawButton(&btnAI);
    // 处理点击
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (btnPVP.isHovered) {
            vsAI = false;
            gameState = STATE_PLAYING;
            InitBoard();
        }
        else if (btnAI.isHovered) {
            vsAI = true;
            gameState = STATE_PLAYING;
            InitBoard();
            currentPlayer = PIECE_BLACK; // 玩家先手
        }
    }
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Gomoku AI");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        if (gameState == STATE_MENU) {
            DrawMenu();
        } 
        else {
            // 游戏逻辑
            if (gameState == STATE_PLAYING) {
                if (vsAI && currentPlayer == PIECE_WHITE) {
                    AIMove();
                }
                else {
                    Vector2 mousePos = GetMousePosition();
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        int x = (mousePos.x - PADDING) / CELL_SIZE;
                        int y = (mousePos.y - PADDING) / CELL_SIZE;
                        if (x >= 0 && x < BOARD_SIZE && 
                            y >= 0 && y < BOARD_SIZE && 
                            board[y][x] == PIECE_EMPTY) {
                            board[y][x] = currentPlayer;
                            if (CheckWin(x, y)) {
                                gameState = (currentPlayer == PIECE_BLACK) ? 
                                            STATE_BLACK_WIN : STATE_WHITE_WIN;
                            } else {
                                currentPlayer = (currentPlayer == PIECE_BLACK) ? 
                                              PIECE_WHITE : PIECE_BLACK;
                            }
                        }
                    }
                }
            }
            if (IsKeyPressed(KEY_R)) {
                gameState = STATE_MENU;
            }
            // 绘制
            ClearBackground(RAYWHITE);
            DrawBoard();
            DrawGameUI();
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}