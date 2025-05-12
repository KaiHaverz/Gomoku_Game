#include <SFML/Graphics.hpp>
#include "Game.hpp"  // 可选：后续扩展游戏逻辑

int main() {
    // 创建窗口
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML 测试");
    window.setFramerateLimit(60);

    // 加载资源（示例：字体）
    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf")) {
        return EXIT_FAILURE;
    }

    // 测试文本
    sf::Text text("SFML 运行成功！", font, 24);
    text.setPosition(50, 50);

    // 主循环
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // 渲染
        window.clear(sf::Color::Black);
        window.draw(text);
        window.display();
    }

    return 0;
}