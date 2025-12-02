#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Coral {
private:
    static sf::Texture sharedTextures[4];  // 4 texturas para 4 tipos de coral
    sf::Sprite sprite;
    bool alive;

public:
    // Cargar las 4 texturas de coral (una sola vez)
    static bool loadTextures(const std::string& path1, const std::string& path2,
                             const std::string& path3, const std::string& path4);

    // Crear coral estático en una posición
    Coral(const sf::Vector2f& position, int coralType);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    bool isAlive() const;
    sf::FloatRect getGlobalBounds() const;
};
