// include/button.hpp
#ifndef GOMOKU_BUTTON_HPP
#define GOMOKU_BUTTON_HPP

#include <raylib.h>

struct Button {
    Rectangle bounds;
    const char* text;
    bool isHovered;
};

Button CreateButton(float x, float y, float width, float height, const char* text);
void DrawButton(Button* button);

#endif // GOMOKU_BUTTON_HPP