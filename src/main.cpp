#include <raylib.h>

const int BOARD_SIZE = 15;
const int CELL_SIZE = 40;
const int OFFSET_X = 50;
const int OFFSET_Y = 50;

int main() {
    // 初始化窗口
    InitWindow(800, 600, "五子棋 - Raylib");
    SetTargetFPS(60);

    // 游戏主循环
    while (!WindowShouldClose()) {
        // 输入处理
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            int boardX = (mousePos.x - OFFSET_X) / CELL_SIZE;
            int boardY = (mousePos.y - OFFSET_Y) / CELL_SIZE;
            // 调试输出坐标
            TraceLog(LOG_INFO, TextFormat("点击坐标: (%d, %d)", boardX, boardY));
        }

        // 渲染
        BeginDrawing();
            ClearBackground(RAYWHITE); // 使用 RAYWHITE 替代直接颜色值
            
            // 绘制棋盘网格（使用 Color{} 显式构造）
            for (int i = 0; i < BOARD_SIZE; i++) {
                DrawLine(
                    OFFSET_X, OFFSET_Y + i * CELL_SIZE,
                    OFFSET_X + (BOARD_SIZE-1)*CELL_SIZE, OFFSET_Y + i * CELL_SIZE,
                    Color{0, 0, 0, 255}  // 显式构造 BLACK
                );
                DrawLine(
                    OFFSET_X + i * CELL_SIZE, OFFSET_Y,
                    OFFSET_X + i * CELL_SIZE, OFFSET_Y + (BOARD_SIZE-1)*CELL_SIZE,
                    Color{0, 0, 0, 255}  // 显式构造 BLACK
                );
            }

            // 显示提示文字
            DrawText("点击鼠标落子", 10, 10, 20, Color{0, 0, 0, 255});
        EndDrawing();
    }

    CloseWindow();
    return 0;
}