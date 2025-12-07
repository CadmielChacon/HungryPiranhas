#include "../include/Bubble.hpp" // Ajusta la ruta si Bubble.hpp no está en include/
#include <iostream> // Para depuración, si es necesario

// Inicializar la textura estática
sf::Texture Bubble::sharedTexture;

bool Bubble::loadTexture(const std::string& texturePath) {
    return sharedTexture.loadFromFile(texturePath);
}

// Constructor
Bubble::Bubble(const sf::Vector2f& startPosition, const std::string& texturePath,
               float initialLifeTime, float initialUpwardSpeed)
    : lifeTime(initialLifeTime), // Inicializa el tiempo de vida
      initialLifeTime(initialLifeTime), // Guarda el tiempo de vida inicial
      upwardSpeed(initialUpwardSpeed), // Velocidad de subida
      alive(true) // La burbuja nace viva
{
    // Usar la textura compartida
    sprite.setTexture(sharedTexture);

    // Ajustar el origen al centro para que la posición sea el centro de la burbuja
    sf::Vector2u textureSize = sharedTexture.getSize();
    sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);

    // Establecer la posición inicial
    sprite.setPosition(startPosition);

    // Escalar la burbuja si es muy grande/pequeña
    sprite.setScale(1.5f, 1.5f); 
}

void Bubble::update(float deltaTime) {
    if (!alive) return; // Si ya no está viva, no hacer nada

    // Mover la burbuja hacia arriba
    sprite.move(0.0f, -upwardSpeed * deltaTime);

    // Reducir el tiempo de vida
    lifeTime -= deltaTime;

    // Calcular la opacidad (alpha) basada en el tiempo de vida restante
    // Queremos que se desvanezca al final de su vida
    if (lifeTime <= 0) {
        alive = false; // La burbuja ha muerto
        
    } else {
        // Normalizar lifeTime a un rango [0, 1] para calcular la opacidad
        float normalizedLife = lifeTime / initialLifeTime;
        // Asegurar que normalizedLife está entre 0 y 1
        if (normalizedLife < 0.0f) normalizedLife = 0.0f;
        if (normalizedLife > 1.0f) normalizedLife = 1.0f;
        
        int alphaInt = static_cast<int>(normalizedLife * 255.0f);
        sf::Uint8 alpha = static_cast<sf::Uint8>(alphaInt);
        sprite.setColor(sf::Color(255, 255, 255, alpha));
    }
}

void Bubble::draw(sf::RenderWindow& window) {
    if (alive) { // Solo dibujar si está viva
        window.draw(sprite);
    }
}

bool Bubble::isAlive() const {
    return alive;
}

sf::Vector2f Bubble::getPosition() const {
    return sprite.getPosition();
}
