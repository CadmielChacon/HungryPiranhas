#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Piranha {
private:
    static sf::Texture sharedTexture;
    sf::Sprite sprite;
    
    // Posición original 
    sf::Vector2f originalPosition;
    
    // Estados y movimiento
    enum State { IDLE, ALERT, MOVING, RETURNING };
    State state;
    float moveSpeed;
    float maxMoveDistance;
    
    // Temporizadores individuales
    sf::Clock stateTimer;
    sf::Clock alertBlinkTimer;
    sf::Clock attackCycleTimer;  // temporizador para decidir cuándo atacar
    
    // Duraciones de estados
    float alertDuration;
    float moveDuration;
    float idleDuration;
    float alertBlinkInterval;
    float attackCycleInterval;  // tiempo entre ciclos de ataque 
    
    // Flag para saber si esta piraña debe atacar en este ciclo
    bool shouldAttackThisCycle;

public:
    // Cargar la textura compartida 
    static bool loadTexture(const std::string& path);

    // Crear piraña en una posicion
    Piranha(const sf::Vector2f& position);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    
    // Obtener información de estado
    sf::FloatRect getGlobalBounds() const;
    bool isMoving() const;
    bool isAlert() const;
    sf::Vector2f getAlertBubblePosition() const;
};
