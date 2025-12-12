#pragma once
#include <SFML/Graphics.hpp>

class Fish {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    const float max_speed = 5.0f; // definir la velocidad
    sf::Clock bubbleTimer; // Temporizador para controlar la frecuencia de las burbujas
    // Intervalos para la emisión de burbujas:
    float minBubbleInterval = 0.5f; 
    float maxBubbleInterval = 0.05f;  
    float distanceToTarget = 0.0f; // distancia actual al cursor 
    bool isMoving = false; // Estado de movimiento del pez
    // Vidas del pez
    static const int MAX_LIVES = 10;
    int lives = MAX_LIVES;

    // Cooldown para evitar recibir daño continuo al estar en contacto
    float damageCooldownSeconds = 1.0f;
    sf::Clock damageClock;

    // Sistema de daño con parpadeo y invulnerabilidad temporal
    bool m_isHit = false;
    sf::Clock m_hitTimer;
    static constexpr float HIT_DURATION = 2.0f;

public:
    Fish();
    bool load(const std::string& path);
    void draw(sf::RenderWindow& window);
    void update(const sf::RenderWindow& window); 
    
    // Devuelve la posición de la cola para crear la burbuja
    sf::Vector2f getTailPosition() const;

    // Comprueba si es hora de crear una burbuja
    bool shouldEmitBubble() const;

    // Los metodos get/move siguen siendo necesarios para la implementacion interna
    sf::Vector2f getPosition() const;
    void move(const sf::Vector2f& offset);
    void restartBubbleTimer();

    // Vidas y daño
    int getLives() const;
    bool tryTakeDamage(int amount);
    int getMaxLives() const;

    // Sistema de hit (daño + parpadeo temporal)
    void hit();
    bool isHit() const { return m_isHit; }

    // Obtener bounds para colisiones
    sf::FloatRect getGlobalBounds() const;
    sf::FloatRect getHitbox() const; // Hitbox reducida para colisiones más precisas

    // Reiniciar el estado del pez
    void reset();
};
