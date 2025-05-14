// src/button.cpp
#include "button.hpp"
#include <raylib.h>

Button CreateButton(float x, float y, float width, float height, const char* text) {
    return Button{
        .bounds = {x, y, width, height},
        .text = text,
        .isHovered = false
    };
}

// 绘制按钮
void DrawButton(Button *button)
{
    Color btnColor = button->isHovered ? SKYBLUE : LIGHTGRAY;
    DrawRectangleRec(button->bounds, btnColor);
    DrawRectangleLinesEx(button->bounds, 2, DARKGRAY);

    int textWidth = MeasureText(button->text, 30);
    DrawText(button->text,
             button->bounds.x + (button->bounds.width - textWidth) / 2,
             button->bounds.y + 15,
             30, DARKGRAY);
}
