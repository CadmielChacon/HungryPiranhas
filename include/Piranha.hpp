#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Piranha {
private:
    static sf::Texture sharedTexture;
    sf::Sprite sprite;
    
    // Posición original (donde está de reposo)
    sf::Vector2f originalPosition;
    
    // Estados y movimiento
    enum State { IDLE, ALERT, MOVING, RETURNING };
    State state;
    float moveSpeed;
    float maxMoveDistance;  // distancia hacia la derecha
    
    // Temporizadores
    sf::Clock stateTimer;
    sf::Clock alertBlinkTimer;
    
    // Duraciones de estados
    float alertDuration;      // tiempo de parpadeo antes de mover
    float moveDuration;       // tiempo de movimiento
    float idleDuration;       // tiempo entre ciclos
    float alertBlinkInterval; // intervalo de parpadeo

public:
    // Cargar la textura compartida (una sola vez)
    static bool loadTexture(const std::string& path);

    // Crear piraña en una posición
    Piranha(const sf::Vector2f& position);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    
    // Obtener información de estado
    sf::FloatRect getGlobalBounds() const;
    bool isMoving() const;
    bool isAlert() const;
    sf::Vector2f getAlertBubblePosition() const;
};
