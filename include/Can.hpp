#ifndef CAN_HPP
#define CAN_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Can {
private:
    sf::Sprite sprite;
    sf::Vector2f position;
    float moveSpeed;  // píxeles por segundo 
    bool isAlive;
    int canType;  // 0 = can1, 1 = can2, 2 = can3
    
    static sf::Texture sharedTextures[3];  // Texturas compartidas para los 3 tipos de latas

public:
    Can(const sf::Vector2f& startPos, int type);
    
    static bool loadTextures(const std::string& path1, const std::string& path2, const std::string& path3);
    
    void update(float deltaTime, float difficultyMultiplier = 1.0f);
    void draw(sf::RenderWindow& window);
    
    sf::FloatRect getGlobalBounds() const;
    bool getIsAlive() const;
    void setIsAlive(bool alive);
    
    sf::Vector2f getPosition() const;
};

#endif
