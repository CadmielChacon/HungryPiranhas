#include "../include/LivesHUD.hpp"
#include <iostream>

LivesHUD::LivesHUD() {}

bool LivesHUD::load(const std::string& path) {
    if (!texture.loadFromFile(path)) {
        std::cout << "ERROR: No se pudo cargar HUD de vidas\n";
        return false;
    }
    sprite.setTexture(texture);
    sprite.setPosition(10.f, 10.f);
    sprite.setScale(5.0f, 5.0f);
    return true;
}

void LivesHUD::update(int lives) {
    if (lives < 0) lives = 0;
    if (lives > MAX_LIVES) lives = MAX_LIVES;

    sprite.setTextureRect(sf::IntRect(
        FRAME_WIDTH * (MAX_LIVES - lives),
        0,
        FRAME_WIDTH,
        FRAME_HEIGHT
    ));
}

void LivesHUD::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::Vector2f LivesHUD::getPosition() const {
    return sprite.getPosition();
}

float LivesHUD::getWidth() const {
    return sprite.getGlobalBounds().width;
}
