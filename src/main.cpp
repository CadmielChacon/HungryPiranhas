#include <SFML/Graphics.hpp>
#include "../include/Fish.hpp"
#include "../include/Bubble.hpp" 
#include "../include/Piranha.hpp"
#include "../include/Coral.hpp"
#include "../include/Can.hpp"
#include "../include/LivesHUD.hpp"
#include <iostream>
#include <vector>
#include <random>

sf::Clock gameClock;
int main() {
    sf::RenderWindow window(sf::VideoMode(1300, 700), "Hungry Piranhas");

    // OCULTAR EL CURSOR
    window.setMouseCursorVisible(false);

    Fish fish;
    
    // VERIFICACIÓN CRÍTICA
    if (!fish.load("../assets/fish.png")) {
        // Si la carga falla, detiene la ejecución inmediatamente.
        std::cerr << "Error Crítico: Terminando el programa porque no se cargó 'fish.png'.\n";
        return 1; 
    }

    // Cargar la textura de las burbujas una sola vez
    if (!Bubble::loadTexture("../assets/bubble.png")) {
        std::cerr << "Error Crítico: Terminando el programa porque no se cargó 'bubble.png'.\n";
        return 1;
    }

    // Cargar textura de pirañas
    if (!Piranha::loadTexture("../assets/piranha.png")) {
        std::cerr << "Error Crítico: Terminando el programa porque no se cargó 'piranha.png'.\n";
        return 1;
    }

    // Cargar texturas de corales
    if (!Coral::loadTextures("../assets/coralP1.png", "../assets/coralP2.png",
                             "../assets/coralR1.png", "../assets/coralR2.png")) {
        std::cerr << "Aviso: no se cargaron todas las texturas de coral.\n";
    } else {
        std::cerr << "Texturas de coral cargadas exitosamente.\n";
    }

    // Cargar texturas de latas
    if (!Can::loadTextures("../assets/can1.png", "../assets/can2.png", "../assets/can3.png")) {
        std::cerr << "Aviso: no se cargaron todas las texturas de latas.\n";
    } else {
        std::cerr << "Texturas de latas cargadas exitosamente.\n";
    }

    // Cargar textura
    LivesHUD livesHUD;
    if (!livesHUD.load("../assets/lives.png")) { // Ajusta la ruta y el nombre del archivo
        std::cerr << "Error Crítico: Terminando el programa porque no se cargó 'lives.png'.\n";
        return 1;
    }

    // Cargar textura de fondo
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("../assets/sea.png")) {
        std::cerr << "Aviso: no se pudo cargar la imagen de fondo. Usando color azul.\n";
    }

    sf::Sprite backgroundSprite(backgroundTexture);

    if (backgroundTexture.getSize().x > 0) {
        backgroundSprite.setScale(
            float(window.getSize().x) / backgroundTexture.getSize().x,
            float(window.getSize().y) / backgroundTexture.getSize().y
        );
    }

    // Contenedor para las burbujas
    std::vector<Bubble> bubbles;
    // Crear columna de pirañas en el borde derecho (varias)
    std::vector<Piranha> piranhas;
    const int pCount = 6;
    float marginRight = 60.0f;
    float startY = 80.0f;
    float spacing = 90.0f;
    float xPos = static_cast<float>(window.getSize().x) - marginRight;
    for (int i = 0; i < pCount; ++i) {
        piranhas.emplace_back(sf::Vector2f(xPos, startY + i * spacing));
    }

    // Contenedor para corales
    std::vector<Coral> corals;
    sf::Clock coralSpawnTimer;
    float coralSpawnInterval = 2.0f;  // cada 2 segundos
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> coralTypeDist(0, 3);  // tipo de coral [0, 3]
    std::uniform_real_distribution<> coralXDist(50.0f, 1250.0f);  // posición X aleatoria
    float coralY = 610.0f;  // posición Y fija

    // Contenedor para latas
    std::vector<Can> cans;
    sf::Clock canSpawnTimer;
    float canSpawnInterval = 1.5f;  // cada 1.5 segundos
    std::uniform_int_distribution<> canTypeDist(0, 2);  // tipo de lata [0, 1, 2]
    std::uniform_real_distribution<> canXDist(50.0f, 1250.0f);  // posición X aleatoria

    // Inicialización completa
    while (window.isOpen()) {
        
        // Calcular el tiempo que pasó desde el último frame
        sf::Time dt = gameClock.restart();
        float deltaTime = dt.asSeconds(); // DeltaTime en segundos
        
        // Manejo de eventos
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        // Lógica
        fish.update(window); 
        livesHUD.update(fish.getLives());

        // Actualizar pirañas
        for (size_t i = 0; i < piranhas.size(); ++i) {
            piranhas[i].update(deltaTime);
        }

        // Colisiones con pirañas
        for (size_t i = 0; i < piranhas.size(); ++i) {
            if (fish.getGlobalBounds().intersects(piranhas[i].getGlobalBounds())) {
                if (fish.tryTakeDamage(1)) {
                    livesHUD.update(fish.getLives());
                    std::cerr << "Fish hit! Lives: " << fish.getLives() << std::endl;
                    if (fish.getLives() <= 0) {
                        std::cerr << "Fish died. Game over." << std::endl;
                        window.close();
                    }
                    break; // evitar múltiples daños en la misma actualización
                }
            }
        }

        // Spawn aleatorio de corales
        if (coralSpawnTimer.getElapsedTime().asSeconds() >= coralSpawnInterval) {
            int type = coralTypeDist(gen);
            float xPos = coralXDist(gen);
            corals.emplace_back(sf::Vector2f(xPos, coralY), type);
            std::cerr << "Coral spawned at X: " << xPos << "! Total corals: " << corals.size() << std::endl;
            coralSpawnTimer.restart();
        }

        // Actualizar corales
        for (size_t i = 0; i < corals.size(); ++i) {
            corals[i].update(deltaTime);
        }

        // Colisiones con corales
        for (size_t i = 0; i < corals.size(); ++i) {
            if (corals[i].isAlive() && fish.getGlobalBounds().intersects(corals[i].getGlobalBounds())) {
                if (fish.tryTakeDamage(1)) {
                    livesHUD.update(fish.getLives());
                    std::cerr << "Fish hit by coral! Lives: " << fish.getLives() << std::endl;
                    // Marcar coral como muerto para no causar daño otra vez
                    corals[i] = Coral(sf::Vector2f(-1000.0f, -1000.0f), 0);
                    if (fish.getLives() <= 0) {
                        std::cerr << "Fish died. Game over." << std::endl;
                        window.close();
                    }
                    break;
                }
            }
        }

        // Limpieza de corales muertos
        for (int i = corals.size() - 1; i >= 0; --i) {
            if (!corals[i].isAlive()) {
                corals.erase(corals.begin() + i);
            }
        }

        // Spawn aleatorio de latas
        if (canSpawnTimer.getElapsedTime().asSeconds() >= canSpawnInterval) {
            int type = canTypeDist(gen);
            float xPos = canXDist(gen);
            cans.emplace_back(sf::Vector2f(xPos, -30.0f), type);  // Spawn en la parte superior
            std::cerr << "Can spawned at X: " << xPos << " Type: " << type << "! Total cans: " << cans.size() << std::endl;
            canSpawnTimer.restart();
        }

        // Actualizar latas
        for (size_t i = 0; i < cans.size(); ++i) {
            cans[i].update(deltaTime);
        }

        // Colisiones con latas
        for (size_t i = 0; i < cans.size(); ++i) {
            if (cans[i].getIsAlive() && fish.getGlobalBounds().intersects(cans[i].getGlobalBounds())) {
                if (fish.tryTakeDamage(1)) {
                    livesHUD.update(fish.getLives());
                    std::cerr << "Fish hit by can! Lives: " << fish.getLives() << std::endl;
                    // Marcar lata como muerta para no causar daño otra vez
                    cans[i].setIsAlive(false);
                    if (fish.getLives() <= 0) {
                        std::cerr << "Fish died. Game over." << std::endl;
                        window.close();
                    }
                    break;
                }
            }
        }

        // Limpieza de latas muertas
        for (int i = cans.size() - 1; i >= 0; --i) {
            if (!cans[i].getIsAlive()) {
                cans.erase(cans.begin() + i);
            }
        }

        if (fish.shouldEmitBubble()) {
            // Creamos una burbuja en la posición de la cola del pez
            // (Vida útil: 2 segundos, Velocidad de subida: 150px/s)
            bubbles.emplace_back(fish.getTailPosition(), "", 2.0f, 150.0f); 
            fish.restartBubbleTimer();
            
        }

        // Actualizar todas las burbujas existentes
        for (size_t i = 0; i < bubbles.size(); ++i) {
            bubbles[i].update(deltaTime);
        }
        
        // Limpieza de burbujas muertas
        for (int i = bubbles.size() - 1; i >= 0; --i) {
            if (!bubbles[i].isAlive()) {
                bubbles.erase(bubbles.begin() + i);
            }
        }

        // Dibujo
        window.clear();
        window.draw(backgroundSprite);
        
        // Dibujar burbujas antes del pez
        for (size_t i = 0; i < bubbles.size(); ++i) {
            bubbles[i].draw(window);
        }

        // Dibujar corales
        for (size_t i = 0; i < corals.size(); ++i) {
            corals[i].draw(window);
        }

        // Dibujar latas
        for (size_t i = 0; i < cans.size(); ++i) {
            cans[i].draw(window);
        }
        
        fish.draw(window);

        // Dibujar pirañas encima de todo
        for (size_t i = 0; i < piranhas.size(); ++i) {
            piranhas[i].draw(window);
        }

        // HUD de vidas
        livesHUD.draw(window);

        window.display();
    }

    return 0;
}
