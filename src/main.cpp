#include <raylib.h>

// 游戏常量
const int BOARD_SIZE = 15;
const int CELL_SIZE = 40;
const int PADDING = 50;
const int WINDOW_WIDTH = PADDING * 2 + (BOARD_SIZE - 1) * CELL_SIZE;  // 50 * 2 + 14 * 40 = 660
const int WINDOW_HEIGHT = PADDING * 2 + (BOARD_SIZE - 1) * CELL_SIZE; // 50 * 2 + 14 * 40 = 660

// 颜色定义
const Color BOARD_COLOR = { 210, 180, 140, 255 }; // 木质色
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

// 全局变量
Piece board[BOARD_SIZE][BOARD_SIZE];
Piece currentPlayer = PIECE_BLACK;
GameState gameState = STATE_MENU;

// 按钮结构体
struct Button {
    Rectangle bounds;
    const char* text;
    bool isHovered;
};

// 初始化按钮
Button CreateButton(float x, float y, float width, float height, const char* text) {
    return (Button){
        .bounds = {x, y, width, height},
        .text = text,
        .isHovered = false
    };
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
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            board[y][x] = PIECE_EMPTY;
        }
    }
    currentPlayer = PIECE_BLACK;
}

// 绘制棋盘
void DrawBoard() {
    // 1. 绘制棋盘背景
    DrawRectangle(
        PADDING - 10, 
        PADDING - 10,
        (BOARD_SIZE-1)*CELL_SIZE + 20, 
        (BOARD_SIZE-1)*CELL_SIZE + 20,
        BOARD_COLOR
    );

    // 2. 绘制网格线
    for (int i = 0; i < BOARD_SIZE; i++) {
        // 横线
        DrawLineEx(
            Vector2{ (float)PADDING, (float)(PADDING + i*CELL_SIZE) },
            Vector2{ (float)(PADDING + (BOARD_SIZE-1)*CELL_SIZE), (float)(PADDING + i*CELL_SIZE) },
            1.5f, 
            LINE_COLOR
        );
        // 竖线
        DrawLineEx(
            Vector2{ (float)(PADDING + i*CELL_SIZE), (float)PADDING },
            Vector2{ (float)(PADDING + i*CELL_SIZE), (float)(PADDING + (BOARD_SIZE-1)*CELL_SIZE) },
            1.5f, 
            LINE_COLOR
        );
    }

    // 3. 绘制星位点（天元+四角）
    const int starPoints[5][2] = {{3,3}, {11,3}, {3,11}, {11,11}, {7,7}};
    for (const auto& point : starPoints) {
        DrawCircle(
            PADDING + point[0]*CELL_SIZE,
            PADDING + point[1]*CELL_SIZE,
            5, 
            LINE_COLOR
        );
    }

    // 4. 绘制棋子
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x] == PIECE_BLACK) {
                DrawCircle(
                    PADDING + x*CELL_SIZE,
                    PADDING + y*CELL_SIZE,
                    CELL_SIZE/2 - 2, 
                    BLACK
                );
            } 
            else if (board[y][x] == PIECE_WHITE) {
                DrawCircle(
                    PADDING + x*CELL_SIZE,
                    PADDING + y*CELL_SIZE,
                    CELL_SIZE/2 - 2, 
                    WHITE
                );
                DrawCircleLines(
                    PADDING + x*CELL_SIZE,
                    PADDING + y*CELL_SIZE,
                    CELL_SIZE/2 - 2, 
                    BLACK
                );
            }
        }
    }
}

// 检查五子连珠
bool CheckWin(int x, int y) {
    if (board[y][x] == PIECE_EMPTY) return false;

    const int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
    for (const auto& dir : dirs) {
        int count = 1;
        int dx = dir[0], dy = dir[1];
        
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

// 绘制游戏UI
void DrawGameUI() {
    // 当前玩家提示
    const char* text = (currentPlayer == PIECE_BLACK) ? "NOW: BLACK" : "NOW: WHITE";
    DrawText(text, 20, 20, 20, TEXT_COLOR);

    // 胜利提示
    if (gameState == STATE_BLACK_WIN) {
        DrawText("BLACK WIN!", 
            WINDOW_WIDTH/2 - MeasureText("BLACK WIN!", 40)/2,
            WINDOW_HEIGHT/2 - 20, 
            40, 
            RED
        );
    } 
    else if (gameState == STATE_WHITE_WIN) {
        DrawText("WHITE WIN!", 
            WINDOW_WIDTH/2 - MeasureText("WHITE WIN!", 40)/2,
            WINDOW_HEIGHT/2 - 20, 
            40, 
            RED
        );
    }

    // 操作提示
    DrawText("Press 'R' to return to menu", 
        20, 
        WINDOW_HEIGHT - 30, 
        20, 
        DARKGRAY
    );
}

// 绘制开始菜单
void DrawMenu() {
    ClearBackground(RAYWHITE);
    
    // 绘制标题
    DrawText("Gomoku Game", 
            WINDOW_WIDTH/2 - MeasureText("Gomoku Game", 50)/2, 
            150, 50, BLACK);
    
    // 创建并绘制按钮
    Button btnPair = CreateButton(
        WINDOW_WIDTH/2 - 100, 
        WINDOW_HEIGHT/2 - 30, 
        200, 60, "Pair Game"
    );
    
    // 更新按钮状态
    btnPair.isHovered = CheckCollisionPointRec(GetMousePosition(), btnPair.bounds);
    
    // 绘制按钮
    DrawButton(&btnPair);
    
    // 检测点击
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && btnPair.isHovered) {
        gameState = STATE_PLAYING;
        InitBoard();
    }
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Gomoku Game");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        
        if (gameState == STATE_MENU) {
            DrawMenu();
        } 
        else {
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
            ClearBackground(RAYWHITE);
            DrawBoard();
            DrawGameUI();
        }
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}