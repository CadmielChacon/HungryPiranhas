#include "../include/Piranha.hpp"
#include <iostream>
#include <cmath>

sf::Texture Piranha::sharedTexture;

bool Piranha::loadTexture(const std::string& path) {
    return sharedTexture.loadFromFile(path);
}

Piranha::Piranha(const sf::Vector2f& position)
    : originalPosition(position), state(IDLE), moveSpeed(200.0f), 
      maxMoveDistance(250.0f), alertDuration(2.0f), moveDuration(1.5f), 
      idleDuration(4.0f), alertBlinkInterval(0.3f) {
    
    sprite.setTexture(sharedTexture);
    // Ajustar origen al centro
    sf::Vector2u texSize = sharedTexture.getSize();
    sprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
    sprite.setPosition(position);
    // Escalar la piraña si es muy grande/pequeña
    sprite.setScale(5.0f, 5.0f);
    
    stateTimer.restart();
}

void Piranha::update(float deltaTime) {
    float elapsedTime = stateTimer.getElapsedTime().asSeconds();
    
    switch (state) {
        case IDLE:
            if (elapsedTime >= idleDuration) {
                state = ALERT;
                stateTimer.restart();
                alertBlinkTimer.restart();
            }
            break;
            
        case ALERT:
            if (elapsedTime >= alertDuration) {
                state = MOVING;
                stateTimer.restart();
            }
            break;
            
        case MOVING: {
            // Moverse hacia la izquierda (desde su posición original hacia la izquierda)
            float progress = elapsedTime / moveDuration;
            if (progress >= 1.0f) {
                state = RETURNING;
                stateTimer.restart();
            } else {
                float moveDistance = maxMoveDistance * progress;
                // restar para moverse a la izquierda
                sprite.setPosition(originalPosition.x - moveDistance, originalPosition.y);
            }
            break;
        }
            
        case RETURNING: {
            // Regresar a posición original (desde la izquierda hacia la original)
            float progress = elapsedTime / moveDuration;
            if (progress >= 1.0f) {
                sprite.setPosition(originalPosition);
                state = IDLE;
                stateTimer.restart();
            } else {
                float moveDistance = maxMoveDistance * (1.0f - progress);
                // durante el retorno, la posición es original - moveDistance
                sprite.setPosition(originalPosition.x - moveDistance, originalPosition.y);
            }
            break;
        }
    }
}

void Piranha::draw(sf::RenderWindow& window) {
    // Si está en estado ALERT, hacer parpadeo
    if (state == ALERT) {
        float blinkTime = alertBlinkTimer.getElapsedTime().asSeconds();
        float cycle = std::fmod(blinkTime, alertBlinkInterval * 2.0f);
        // Mostrar/ocultar sprite alternativamente
        if (cycle < alertBlinkInterval) {
            window.draw(sprite);
        }
    } else {
        window.draw(sprite);
    }
}

sf::FloatRect Piranha::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}

bool Piranha::isMoving() const {
    return state == MOVING || state == RETURNING;
}

bool Piranha::isAlert() const {
    return state == ALERT;
}

sf::Vector2f Piranha::getAlertBubblePosition() const {
    return sf::Vector2f(originalPosition.x - 60.0f, originalPosition.y - 80.0f);
}
