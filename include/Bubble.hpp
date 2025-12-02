#pragma once
#include <SFML/Graphics.hpp>
#include <string> // Necesario para std::string

class Bubble {
private:
    static sf::Texture sharedTexture; // Textura compartida entre todas las burbujas
    sf::Sprite sprite;   // Sprite de la burbuja

    float lifeTime;      // Tiempo de vida restante de la burbuja (en segundos)
    float initialLifeTime; // Tiempo de vida inicial para normalización
    float upwardSpeed;   // Velocidad a la que la burbuja sube

    bool alive;          // Estado para saber si la burbuja está "viva"

public:
    // Cargar la textura compartida una sola vez
    static bool loadTexture(const std::string& texturePath);
    // Constructor: Inicializa la burbuja en una posición y con su textura.
    Bubble(const sf::Vector2f& startPosition, const std::string& texturePath, 
           float initialLifeTime, float initialUpwardSpeed);

    // Actualiza el estado de la burbuja (movimiento, desvanecimiento).
    void update(float deltaTime); 

    // Dibuja la burbuja en la ventana.
    void draw(sf::RenderWindow& window);

    // Comprueba si la burbuja sigue viva.
    bool isAlive() const;

    // Obtiene la posición de la burbuja (útil para depuración).
    sf::Vector2f getPosition() const;
};
