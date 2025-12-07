#pragma once
#include <SFML/Graphics.hpp>

class LivesHUD {
private:
    sf::Texture texture;
    sf::Sprite sprite;

    static const int FRAME_WIDTH = 41;
    static const int FRAME_HEIGHT = 12;
    static const int MAX_LIVES = 10;

public:
    LivesHUD();

    bool load(const std::string& path);

    void update(int lives);
    void draw(sf::RenderWindow& window);
};

