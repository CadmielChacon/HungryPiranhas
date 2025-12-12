#pragma once
#include <SFML/Graphics.hpp>

class Heart {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Clock animationClock;
    int currentFrame;
    static const int FRAME_WIDTH = 10;
    static const int FRAME_HEIGHT = 9;
    static const int TOTAL_FRAMES = 4; // Numero total de frames en la animacion
    bool isVisible;
    float fallSpeed; // Velocidad de caída en píxeles por segundo (no se ve afectada por dificultad)

public:
    Heart();

    bool loadTexture(const std::string& path);
    void spawn(const sf::Vector2f& position);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getGlobalBounds() const;
    bool getIsVisible() const;
    void setIsVisible(bool visible);
};