#include "../include/Coral.hpp"
#include <iostream>

sf::Texture Coral::sharedTextures[4];

bool Coral::loadTextures(const std::string& path1, const std::string& path2,
                         const std::string& path3, const std::string& path4) {
    bool success = true;
    if (!sharedTextures[0].loadFromFile(path1)) {
        std::cerr << "Error: no se pudo cargar coral 1: " << path1 << std::endl;
        success = false;
    }
    if (!sharedTextures[1].loadFromFile(path2)) {
        std::cerr << "Error: no se pudo cargar coral 2: " << path2 << std::endl;
        success = false;
    }
    if (!sharedTextures[2].loadFromFile(path3)) {
        std::cerr << "Error: no se pudo cargar coral 3: " << path3 << std::endl;
        success = false;
    }
    if (!sharedTextures[3].loadFromFile(path4)) {
        std::cerr << "Error: no se pudo cargar coral 4: " << path4 << std::endl;
        success = false;
    }
    return success;
}

Coral::Coral(const sf::Vector2f& position, int coralType) : alive(true) {
    
    // Asegurar que coralType está en rango [0, 3]
    if (coralType < 0) coralType = 0;
    if (coralType > 3) coralType = 3;
    
    sprite.setTexture(sharedTextures[coralType]);
    
    // Ajustar origen al centro
    sf::Vector2u texSize = sharedTextures[coralType].getSize();
    sprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
    
    sprite.setPosition(position);
    sprite.setScale(7.0f, 7.0f);  // Ajusta escala según necesites 
}

void Coral::update(float deltaTime) {
    // Los corales son estáticos, no hay nada que actualizar
    (void)deltaTime;  // evitar warning de parámetro no usado
}

void Coral::draw(sf::RenderWindow& window) {
    if (alive) {
        window.draw(sprite);
    }
}

bool Coral::isAlive() const {
    return alive;
}

sf::FloatRect Coral::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}
