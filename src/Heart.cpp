#include "../include/Heart.hpp"

Heart::Heart() : currentFrame(0), isVisible(false), fallSpeed(200.0f) {}

bool Heart::loadTexture(const std::string& path) {
    if (!texture.loadFromFile(path)) {
        return false;
    }
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(0, 0, FRAME_WIDTH, FRAME_HEIGHT));
    sprite.setScale(3.0f, 3.0f); // Escalar para que sea mas visible
    return true;
}

void Heart::spawn(const sf::Vector2f& position) {
    sprite.setPosition(position);
    isVisible = true;
    currentFrame = 0;
    animationClock.restart();
}

void Heart::update(float deltaTime) {
    if (!isVisible) return;

    // Hacer caer el corazón verticalmente
    sf::Vector2f currentPos = sprite.getPosition();
    currentPos.y += fallSpeed * deltaTime;
    sprite.setPosition(currentPos);

    // Actualizar la animación
    if (animationClock.getElapsedTime().asSeconds() > 0.2f) { // Cambiar frame cada 0.2s
        currentFrame = (currentFrame + 1) % TOTAL_FRAMES;
        sprite.setTextureRect(sf::IntRect(currentFrame * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT));
        animationClock.restart();
    }
}

void Heart::draw(sf::RenderWindow& window) {
    if (isVisible) {
        window.draw(sprite);
    }
}

sf::FloatRect Heart::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}

bool Heart::getIsVisible() const {
    return isVisible;
}

void Heart::setIsVisible(bool visible) {
    isVisible = visible;
}