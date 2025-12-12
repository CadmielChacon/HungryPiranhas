#include "../include/Can.hpp"
#include <iostream>

sf::Texture Can::sharedTextures[3];

bool Can::loadTextures(const std::string& path1, const std::string& path2, const std::string& path3) {
    bool success = true;
    
    if (!sharedTextures[0].loadFromFile(path1)) {
        std::cerr << "Error cargando " << path1 << std::endl;
        success = false;
    }
    if (!sharedTextures[1].loadFromFile(path2)) {
        std::cerr << "Error cargando " << path2 << std::endl;
        success = false;
    }
    if (!sharedTextures[2].loadFromFile(path3)) {
        std::cerr << "Error cargando " << path3 << std::endl;
        success = false;
    }
    
    return success;
}

Can::Can(const sf::Vector2f& startPos, int type)
    : position(startPos), moveSpeed(150.0f), isAlive(true), canType(type) {
    
    // Validar tipo de lata
    if (canType < 0 || canType > 2) {
        canType = 0;
    }
    
    sprite.setTexture(sharedTextures[canType]);
    
    // Ajustar origen al centro
    sf::Vector2u texSize = sharedTextures[canType].getSize();
    sprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
    sprite.setPosition(position);
    
    // Escalar la lata si es muy grande/pequeña
    sprite.setScale(3.0f, 3.0f);
}

void Can::update(float deltaTime, float difficultyMultiplier) {
    if (!isAlive) return;
    
    // Moverse hacia abajo con dificultad escalada
    float scaledSpeed = moveSpeed * difficultyMultiplier;
    position.y += scaledSpeed * deltaTime;
    sprite.setPosition(position);
    
    // Verificar si salio de la ventana
    if (position.y > 700.0f + 50.0f) {  // Un poco mas alla del borde inferior
        isAlive = false;
    }
}

void Can::draw(sf::RenderWindow& window) {
    if (isAlive) {
        window.draw(sprite);
    }
}

sf::FloatRect Can::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}

bool Can::getIsAlive() const {
    return isAlive;
}

void Can::setIsAlive(bool alive) {
    isAlive = alive;
}

sf::Vector2f Can::getPosition() const {
    return position;
}
