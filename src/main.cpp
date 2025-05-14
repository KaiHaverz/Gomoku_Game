#include <raylib.h>
#include <vector>
#include <algorithm>
#include <climits>
#include <stack>

// 游戏常量
const int BOARD_SIZE = 15;
const int CELL_SIZE = 40;
const int PADDING = 50;
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 660;

// 颜色定义
const Color BOARD_COLOR = {210, 180, 140, 255};
const Color LINE_COLOR = BLACK;
const Color TEXT_COLOR = BLACK;
const Color UI_BG_COLOR = {136, 136, 136, 255};

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
    STATE_MENU,
    STATE_PLAYING,
    STATE_BLACK_WIN,
    STATE_WHITE_WIN
};

// 游戏模式
enum GameMode
{
    MODE_PVP,
    MODE_PVE_EASY,
    MODE_PVE_HARD
};

// 全局变量
Piece board[BOARD_SIZE][BOARD_SIZE];
Piece currentPlayer = PIECE_BLACK;
GameState gameState = STATE_MENU;
GameMode gameMode = MODE_PVP;
std::stack<std::pair<int, int>> moveHistory;

// 音效资源
Music bgMusic;
Sound placeSound;
Sound winSound;
Sound aiWinSound;
bool musicPlaying = false;

// 按钮结构体
struct Button
{
    Rectangle bounds;
    const char *text;
    bool isHovered;
};

Button CreateButton(float x, float y, float width, float height, const char *text)
{
    return (Button){{x, y, width, height}, text, false};
}

void DrawButton(Button *button)
{
    Color btnColor = button->isHovered ? SKYBLUE : LIGHTGRAY;
    DrawRectangleRec(button->bounds, btnColor);
    DrawRectangleLinesEx(button->bounds, 2, DARKGRAY);

    int textWidth = MeasureText(button->text, 20);
    DrawText(button->text,
             button->bounds.x + (button->bounds.width - textWidth) / 2,
             button->bounds.y + 10,
             20, DARKGRAY);
}

// 初始化棋盘
void InitBoard()
{
    while (!moveHistory.empty())
        moveHistory.pop();
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            board[y][x] = PIECE_EMPTY;
        }
    }
    currentPlayer = PIECE_BLACK;
}

// 清空棋盘（新功能）
void ClearBoard()
{
    InitBoard();
    gameState = STATE_PLAYING;
}

// 绘制棋盘
void DrawBoard()
{
    // 棋盘背景
    DrawRectangle(
        PADDING - 10,
        PADDING - 10,
        (BOARD_SIZE - 1) * CELL_SIZE + 20,
        (BOARD_SIZE - 1) * CELL_SIZE + 20,
        BOARD_COLOR);

    // 网格线
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        DrawLineEx(
            Vector2{(float)PADDING, (float)(PADDING + i * CELL_SIZE)},
            Vector2{(float)(PADDING + (BOARD_SIZE - 1) * CELL_SIZE), (float)(PADDING + i * CELL_SIZE)},
            1.5f, LINE_COLOR);
        DrawLineEx(
            Vector2{(float)(PADDING + i * CELL_SIZE), (float)PADDING},
            Vector2{(float)(PADDING + i * CELL_SIZE), (float)(PADDING + (BOARD_SIZE - 1) * CELL_SIZE)},
            1.5f, LINE_COLOR);
    }

    // 星位点
    const int starPoints[5][2] = {{3, 3}, {11, 3}, {3, 11}, {11, 11}, {7, 7}};
    for (const auto &point : starPoints)
    {
        DrawCircle(
            PADDING + point[0] * CELL_SIZE,
            PADDING + point[1] * CELL_SIZE,
            5, LINE_COLOR);
    }

    // 棋子
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (board[y][x] == PIECE_BLACK)
            {
                DrawCircle(
                    PADDING + x * CELL_SIZE,
                    PADDING + y * CELL_SIZE,
                    CELL_SIZE / 2 - 2, BLACK);
            }
            else if (board[y][x] == PIECE_WHITE)
            {
                DrawCircle(
                    PADDING + x * CELL_SIZE,
                    PADDING + y * CELL_SIZE,
                    CELL_SIZE / 2 - 2, WHITE);
                DrawCircleLines(
                    PADDING + x * CELL_SIZE,
                    PADDING + y * CELL_SIZE,
                    CELL_SIZE / 2 - 2, BLACK);
            }
        }
    }
}

// 检查胜利条件
bool CheckWin(int x, int y)
{
    if (board[y][x] == PIECE_EMPTY)
        return false;

    const int dirs[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    for (const auto &dir : dirs)
    {
        int count = 1;
        int dx = dir[0], dy = dir[1];

        for (int i = 1; i < 5; i++)
        {
            int nx = x + dx * i, ny = y + dy * i;
            if (nx >= BOARD_SIZE || ny >= BOARD_SIZE || board[ny][nx] != board[y][x])
                break;
            count++;
        }

        for (int i = 1; i < 5; i++)
        {
            int nx = x - dx * i, ny = y - dy * i;
            if (nx < 0 || ny < 0 || board[ny][nx] != board[y][x])
                break;
            count++;
        }

        if (count >= 5)
            return true;
    }
    return false;
}

// 悔棋功能
void UndoMove()
{
    if (moveHistory.empty())
        return;

    auto lastMove = moveHistory.top();
    moveHistory.pop();
    board[lastMove.second][lastMove.first] = PIECE_EMPTY;
    currentPlayer = (currentPlayer == PIECE_BLACK) ? PIECE_WHITE : PIECE_BLACK;

    if ((gameMode == MODE_PVE_EASY || gameMode == MODE_PVE_HARD) &&
        currentPlayer == PIECE_WHITE && !moveHistory.empty())
    {
        lastMove = moveHistory.top();
        moveHistory.pop();
        board[lastMove.second][lastMove.first] = PIECE_EMPTY;
        currentPlayer = PIECE_BLACK;
    }
}

// AI评估函数
int EvaluateBoard()
{
    int score = 0;
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (board[y][x] != PIECE_EMPTY)
            {
                const int dirs[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
                for (const auto &dir : dirs)
                {
                    int dx = dir[0], dy = dir[1];
                    int count = 1;
                    bool blocked = false;

                    for (int i = 1; i < 5; i++)
                    {
                        int nx = x + dx * i, ny = y + dy * i;
                        if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE)
                        {
                            blocked = true;
                            break;
                        }
                        if (board[ny][nx] == board[y][x])
                            count++;
                        else if (board[ny][nx] == PIECE_EMPTY)
                            break;
                        else
                        {
                            blocked = true;
                            break;
                        }
                    }

                    if (count >= 5)
                        return (board[y][x] == PIECE_BLACK) ? INT_MAX : INT_MIN;

                    int value = 0;
                    switch (count)
                    {
                    case 4:
                        value = (blocked) ? 500 : 2000;
                        break;
                    case 3:
                        value = (blocked) ? 100 : 500;
                        break;
                    case 2:
                        value = (blocked) ? 10 : 50;
                        break;
                    case 1:
                        value = 1;
                        break;
                    }

                    if (board[y][x] == PIECE_BLACK)
                        score += value;
                    else
                        score -= value;
                }
            }
        }
    }
    return score;
}

std::vector<std::pair<int, int>> GetPossibleMoves()
{
    std::vector<std::pair<int, int>> moves;
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (board[y][x] != PIECE_EMPTY)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    for (int dx = -1; dx <= 1; dx++)
                    {
                        if (dx == 0 && dy == 0)
                            continue;
                        int nx = x + dx, ny = y + dy;
                        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
                            board[ny][nx] == PIECE_EMPTY)
                        {
                            bool exists = false;
                            for (const auto &move : moves)
                            {
                                if (move.first == nx && move.second == ny)
                                {
                                    exists = true;
                                    break;
                                }
                            }
                            if (!exists)
                                moves.emplace_back(nx, ny);
                        }
                    }
                }
            }
        }
    }
    if (moves.empty())
        moves.emplace_back(BOARD_SIZE / 2, BOARD_SIZE / 2);
    return moves;
}

int Minimax(int depth, int alpha, int beta, bool maximizingPlayer)
{
    if (depth == 0)
        return EvaluateBoard();

    auto moves = GetPossibleMoves();
    if (moves.empty())
        return EvaluateBoard();

    if (maximizingPlayer)
    {
        int maxEval = INT_MIN;
        for (const auto &move : moves)
        {
            int x = move.first, y = move.second;
            board[y][x] = PIECE_BLACK;
            if (CheckWin(x, y))
            {
                board[y][x] = PIECE_EMPTY;
                return INT_MAX;
            }
            int eval = Minimax(depth - 1, alpha, beta, false);
            board[y][x] = PIECE_EMPTY;
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break;
        }
        return maxEval;
    }
    else
    {
        int minEval = INT_MAX;
        for (const auto &move : moves)
        {
            int x = move.first, y = move.second;
            board[y][x] = PIECE_WHITE;
            if (CheckWin(x, y))
            {
                board[y][x] = PIECE_EMPTY;
                return INT_MIN;
            }
            int eval = Minimax(depth - 1, alpha, beta, true);
            board[y][x] = PIECE_EMPTY;
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break;
        }
        return minEval;
    }
}

void AIPlay(bool hardMode)
{
    auto moves = GetPossibleMoves();
    if (moves.empty())
        return;

    int bestScore = (currentPlayer == PIECE_BLACK) ? INT_MIN : INT_MAX;
    std::pair<int, int> bestMove = moves[0];

    for (const auto &move : moves)
    {
        int x = move.first, y = move.second;
        board[y][x] = currentPlayer;

        if (CheckWin(x, y))
        {
            board[y][x] = PIECE_EMPTY;
            bestMove = move;
            break;
        }

        int score;
        if (hardMode)
        {
            score = Minimax(3, INT_MIN, INT_MAX, currentPlayer == PIECE_WHITE);
        }
        else
        {
            score = EvaluateBoard();
            if (currentPlayer == PIECE_WHITE)
                score = -score;
        }

        board[y][x] = PIECE_EMPTY;

        if (currentPlayer == PIECE_BLACK)
        {
            if (score > bestScore)
            {
                bestScore = score;
                bestMove = move;
            }
        }
        else
        {
            if (score < bestScore)
            {
                bestScore = score;
                bestMove = move;
            }
        }
    }

    int x = bestMove.first, y = bestMove.second;
    board[y][x] = currentPlayer;
    moveHistory.push(bestMove);
    PlaySound(placeSound);

    if (CheckWin(x, y))
    {
        gameState = (currentPlayer == PIECE_BLACK) ? STATE_BLACK_WIN : STATE_WHITE_WIN;
        if (gameMode == MODE_PVP || currentPlayer == PIECE_BLACK)
        {
            PlaySound(winSound);
        }
        else
        {
            PlaySound(aiWinSound);
        }
    }
    else
    {
        currentPlayer = (currentPlayer == PIECE_BLACK) ? PIECE_WHITE : PIECE_BLACK;
    }
}

// 绘制游戏UI（右侧）
void DrawGameUI()
{
    DrawRectangle(700, 0, 300, WINDOW_HEIGHT, UI_BG_COLOR);

    const char *playerText = (currentPlayer == PIECE_BLACK) ? "Now: Black" : "Now: White";
    DrawText(playerText, 720, 20, 24, TEXT_COLOR);

    const char *modeText = "";
    switch (gameMode)
    {
    case MODE_PVP:
        modeText = "Pair Game";
        break;
    case MODE_PVE_EASY:
        modeText = "AI game(Easy)";
        break;
    case MODE_PVE_HARD:
        modeText = "AI Game(Hard)";
        break;
    }
    DrawText(modeText, 720, 60, 20, TEXT_COLOR);

    // 悔棋按钮
    Button btnUndo = CreateButton(720, 100, 120, 40, "Undo");
    btnUndo.isHovered = CheckCollisionPointRec(GetMousePosition(), btnUndo.bounds);
    DrawButton(&btnUndo);

    // 清空棋盘按钮（新增）
    Button btnClear = CreateButton(720, 150, 120, 40, "Clear Board");
    btnClear.isHovered = CheckCollisionPointRec(GetMousePosition(), btnClear.bounds);
    DrawButton(&btnClear);

    // 返回菜单按钮
    Button btnMenu = CreateButton(720, 200, 120, 40, "Back to Menu");
    btnMenu.isHovered = CheckCollisionPointRec(GetMousePosition(), btnMenu.bounds);
    DrawButton(&btnMenu);

    // 音乐控制按钮
    Button btnMusic = CreateButton(720, 250, 120, 40, musicPlaying ? "Mute" : "Play Music");
    btnMusic.isHovered = CheckCollisionPointRec(GetMousePosition(), btnMusic.bounds);
    DrawButton(&btnMusic);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (btnUndo.isHovered)
        {
            UndoMove();
        }
        else if (btnClear.isHovered)
        { // 新增清空棋盘功能
            ClearBoard();
        }
        else if (btnMenu.isHovered)
        {
            gameState = STATE_MENU;
        }
        else if (btnMusic.isHovered)
        {
            musicPlaying = !musicPlaying;
            if (musicPlaying)
            {
                PlayMusicStream(bgMusic);
            }
            else
            {
                StopMusicStream(bgMusic);
            }
        }
    }

    if (gameState == STATE_BLACK_WIN)
    {
        DrawText("Black Wins!", 720, 320, 30, RED);
    }
    else if (gameState == STATE_WHITE_WIN)
    {
        DrawText("White Wins!", 720, 320, 30, RED);
    }
}

// 绘制开始菜单（优化居中显示）
void DrawMenu()
{
    ClearBackground(RAYWHITE);

    // 居中显示标题
    const char *title = "GOMOKU";
    int titleWidth = MeasureText(title, 50);
    DrawText(title, (WINDOW_WIDTH - titleWidth) / 2, 100, 50, BLACK);

    // 居中排列按钮
    const int buttonWidth = 200;
    const int buttonHeight = 60;
    const int startX = (WINDOW_WIDTH - buttonWidth) / 2;

    Button btnPVP = CreateButton(startX, 200, buttonWidth, buttonHeight, "Pair Game");
    Button btnPVEEasy = CreateButton(startX, 280, buttonWidth, buttonHeight, "AI Game(Easy)");
    Button btnPVEHard = CreateButton(startX, 360, buttonWidth, buttonHeight, "AI Game(Hard)");

    btnPVP.isHovered = CheckCollisionPointRec(GetMousePosition(), btnPVP.bounds);
    btnPVEEasy.isHovered = CheckCollisionPointRec(GetMousePosition(), btnPVEEasy.bounds);
    btnPVEHard.isHovered = CheckCollisionPointRec(GetMousePosition(), btnPVEHard.bounds);

    DrawButton(&btnPVP);
    DrawButton(&btnPVEEasy);
    DrawButton(&btnPVEHard);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (btnPVP.isHovered)
        {
            gameMode = MODE_PVP;
            gameState = STATE_PLAYING;
            InitBoard();
        }
        else if (btnPVEEasy.isHovered)
        {
            gameMode = MODE_PVE_EASY;
            gameState = STATE_PLAYING;
            InitBoard();
        }
        else if (btnPVEHard.isHovered)
        {
            gameMode = MODE_PVE_HARD;
            gameState = STATE_PLAYING;
            InitBoard();
        }
    }
}

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Gomoku Game");
    SetTargetFPS(60);

    // 初始化音频设备
    InitAudioDevice();
    bgMusic = LoadMusicStream("sound/bg_music.MP3");
    placeSound = LoadSound("sound/place.mp3");
    winSound = LoadSound("sound/win.wav");
    aiWinSound = LoadSound("sound/ai_win.MP3");

    bgMusic.looping = true;
    SetMusicVolume(bgMusic, 0.5f);
    SetSoundVolume(placeSound, 0.7f);
    SetSoundVolume(winSound, 0.8f);
    SetSoundVolume(aiWinSound, 0.8f);

    while (!WindowShouldClose())
    {
        if (musicPlaying)
        {
            UpdateMusicStream(bgMusic);
        }

        BeginDrawing();

        if (gameState == STATE_MENU)
        {
            DrawMenu();
        }
        else
        {
            if (gameState == STATE_PLAYING)
            {
                if ((gameMode == MODE_PVE_EASY || gameMode == MODE_PVE_HARD) &&
                    currentPlayer == PIECE_WHITE)
                {
                    AIPlay(gameMode == MODE_PVE_HARD);
                }
                else
                {
                    Vector2 mousePos = GetMousePosition();
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        int x = (mousePos.x - PADDING) / CELL_SIZE;
                        int y = (mousePos.y - PADDING) / CELL_SIZE;

                        if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE &&
                            board[y][x] == PIECE_EMPTY)
                        {
                            board[y][x] = currentPlayer;
                            moveHistory.push({x, y});
                            PlaySound(placeSound);

                            if (CheckWin(x, y))
                            {
                                gameState = (currentPlayer == PIECE_BLACK) ? STATE_BLACK_WIN : STATE_WHITE_WIN;
                                if (gameMode == MODE_PVP || currentPlayer == PIECE_BLACK)
                                {
                                    PlaySound(winSound);
                                }
                                else
                                {
                                    PlaySound(aiWinSound);
                                }
                            }
                            else
                            {
                                currentPlayer = (currentPlayer == PIECE_BLACK) ? PIECE_WHITE : PIECE_BLACK;
                            }
                        }
                    }
                }
            }

            ClearBackground(RAYWHITE);
            DrawBoard();
            DrawGameUI();
        }

        EndDrawing();
    }

    UnloadMusicStream(bgMusic);
    UnloadSound(placeSound);
    UnloadSound(winSound);
    UnloadSound(aiWinSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}