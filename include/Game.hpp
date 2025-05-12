#pragma once
#include <raylib.h>
#include "Board.hpp"
#include "Types.hpp"

// Game.hpp
class Game {
private:
    Board board;
    PieceType currentPlayer; // 当前玩家（黑/白）
public:
    void HandleInput() {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            int x = (mousePos.x - Board::PADDING) / Board::CELL_SIZE;
            int y = (mousePos.y - Board::PADDING) / Board::CELL_SIZE;
            
            if (x >= 0 && x < Board::SIZE && y >= 0 && y < Board::SIZE) {
                board.PlacePiece(x, y, currentPlayer);
                currentPlayer = (currentPlayer == BLACK) ? WHITE : BLACK;
            }
        }
    }
};