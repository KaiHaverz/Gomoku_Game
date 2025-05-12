#pragma once
#include <SFML/Graphics.hpp>

class Game {
public:
    void handleInput(sf::Event event);
    void update();
    void render(sf::RenderWindow& window);
};