#include "../include/Piranha.hpp"
#include <iostream>
#include <cmath>
#include <random>

sf::Texture Piranha::sharedTexture;

bool Piranha::loadTexture(const std::string& path) {
    return sharedTexture.loadFromFile(path);
}

Piranha::Piranha(const sf::Vector2f& position)
    : originalPosition(position), state(IDLE), moveSpeed(1200.0f), 
      maxMoveDistance(1200.0f), alertDuration(2.0f), moveDuration(1.5f), 
      idleDuration(4.0f), alertBlinkInterval(0.3f), attackCycleInterval(6.0f),
      shouldAttackThisCycle(false) {
    
    sprite.setTexture(sharedTexture);
    // Ajustar origen al centro
    sf::Vector2u texSize = sharedTexture.getSize();
    sprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
    sprite.setPosition(position);
    // Escalar la piraña si es muy grande/pequeña
    sprite.setScale(5.0f, 5.0f);
    
    stateTimer.restart();
    attackCycleTimer.restart();
    
    // Inicializar aleatoriamente si va a atacar en este ciclo (~40% probabilidad)
    std::random_device rd;
    std::mt19937 gen(rd() + std::hash<float>{}(position.x) + std::hash<float>{}(position.y));
    std::uniform_real_distribution<> dis(0.0, 1.0);
    shouldAttackThisCycle = (dis(gen) < 0.4);
}

void Piranha::update(float deltaTime) {
    // Verificar ciclo de ataque independiente
    float attackCycleElapsed = attackCycleTimer.getElapsedTime().asSeconds();
    if (attackCycleElapsed >= attackCycleInterval) {
        // Generar nueva decision de ataque para esta piraña
        if (state == IDLE) {
            std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<> dis(0.0, 1.0);
            shouldAttackThisCycle = (dis(gen) < 0.4); // ~40% probabilidad de atacar
        }
        attackCycleTimer.restart();
    }
    
    // Si no esta atacando y esta en IDLE, permanece en IDLE sin cambiar estado
    if (!shouldAttackThisCycle && state == IDLE) {
        return;
    }
    
    // Si esta atacando, continuar con la maquina de estados
    float elapsedTime = stateTimer.getElapsedTime().asSeconds();
    
    switch (state) {
        case IDLE:
            if (shouldAttackThisCycle && elapsedTime >= idleDuration) {
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
            // Moverse hacia la izquierda atravesando toda la ventana
            float progress = elapsedTime / moveDuration;
            if (progress >= 1.0f) {
                // Quedarse detenido en el lado izquierdo
                float finalX = originalPosition.x - maxMoveDistance;
                sprite.setPosition(finalX, originalPosition.y);
                state = RETURNING;
                stateTimer.restart();
            } else {
                float moveDistance = maxMoveDistance * progress;
                // Moverse desde posicion original hacia la izquierda
                sprite.setPosition(originalPosition.x - moveDistance, originalPosition.y);
            }
            break;
        }
            
        case RETURNING: {
            // Regresar desde el lado izquierdo a la posicion original
            float progress = elapsedTime / moveDuration;
            if (progress >= 1.0f) {
                // Regresar a la posicion original
                sprite.setPosition(originalPosition);
                state = IDLE;
                stateTimer.restart();
            } else {
                // Moverse desde la posicion izquierda hasta la original
                float startX = originalPosition.x - maxMoveDistance;
                float returnDistance = maxMoveDistance * progress;
                sprite.setPosition(startX + returnDistance, originalPosition.y);
            }
            break;
        }
    }
}

void Piranha::draw(sf::RenderWindow& window) {
    // Si esta en estado ALERT, hacer parpadeo
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
