#include <SFML/Graphics.hpp>
#include "../include/Fish.hpp"
#include "../include/Bubble.hpp" 
#include "../include/Piranha.hpp"
#include "../include/Coral.hpp"
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

    // Contenedor para burbujas de alerta de pirañas
    std::vector<std::pair<sf::CircleShape, float>> alertBubbles;  // (burbuja, tiempo de vida)

    // Contenedor para corales
    std::vector<Coral> corals;
    sf::Clock coralSpawnTimer;
    float coralSpawnInterval = 2.0f;  // cada 2 segundos
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> coralTypeDist(0, 3);  // tipo de coral [0, 3]
    std::uniform_real_distribution<> coralXDist(50.0f, 1250.0f);  // posición X aleatoria
    float coralY = 610.0f;  // posición Y fija

    // HUD: fuente y texto para vidas
    sf::Font hudFont;
    sf::Text livesText;
    bool hudEnabled = false;
    // Intentar cargar fuentes del sistema Windows
    const char* fontPaths[] = {
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/Arial.ttf",
        "C:/Windows/Fonts/verdana.ttf",
        "C:/Windows/Fonts/Verdana.ttf",
        "../assets/arial.ttf",
        "../assets/Roboto-Regular.ttf"
    };
    
    for (const char* path : fontPaths) {
        if (hudFont.loadFromFile(path)) {
            hudEnabled = true;
            std::cerr << "Fuente cargada: " << path << std::endl;
            break;
        }
    }
    
    if (!hudEnabled) {
        std::cerr << "Aviso: no se pudo cargar la fuente para el HUD. Vidas no mostradas.\n";
    }

    if (hudEnabled) {
        livesText.setFont(hudFont);
        livesText.setCharacterSize(24);
        livesText.setFillColor(sf::Color::White);
        livesText.setOutlineColor(sf::Color::Black);
        livesText.setOutlineThickness(1.0f);
        livesText.setPosition(12.0f, 8.0f);
    }

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

        // Actualizar pirañas
        for (size_t i = 0; i < piranhas.size(); ++i) {
            piranhas[i].update(deltaTime);
            // Si piraña está en alerta y no hay burbuja de alerta, crear una
            if (piranhas[i].isAlert()) {
                // Revisar si ya existe una burbuja de alerta para esta piraña
                bool bubbleExists = false;
                for (const auto& pair : alertBubbles) {
                    if (std::abs(pair.first.getPosition().x - piranhas[i].getAlertBubblePosition().x) < 10.0f) {
                        bubbleExists = true;
                        break;
                    }
                }
                if (!bubbleExists) {
                    // Crear nueva burbuja de alerta
                    sf::CircleShape bubble(15.0f);
                    bubble.setFillColor(sf::Color::Yellow);
                    bubble.setPosition(piranhas[i].getAlertBubblePosition());
                    alertBubbles.emplace_back(bubble, 2.0f);  // 2 segundos de vida
                }
            }
        }

        // Actualizar y limpiar burbujas de alerta
        for (int i = alertBubbles.size() - 1; i >= 0; --i) {
            alertBubbles[i].second -= deltaTime;
            if (alertBubbles[i].second <= 0.0f) {
                alertBubbles.erase(alertBubbles.begin() + i);
            }
        }

        // Colisiones con pirañas
        for (size_t i = 0; i < piranhas.size(); ++i) {
            if (fish.getGlobalBounds().intersects(piranhas[i].getGlobalBounds())) {
                if (fish.tryTakeDamage(1)) {
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
        window.clear(sf::Color::Blue);
        
        // Dibujar burbujas antes del pez
        for (size_t i = 0; i < bubbles.size(); ++i) {
            bubbles[i].draw(window);
        }
        // Dibujar pirañas en la columna derecha
        for (size_t i = 0; i < piranhas.size(); ++i) {
            piranhas[i].draw(window);
        }

        // Dibujar burbujas de alerta
        for (const auto& pair : alertBubbles) {
            window.draw(pair.first);
        }

        // Dibujar corales
        for (size_t i = 0; i < corals.size(); ++i) {
            corals[i].draw(window);
        }
        
        fish.draw(window);
        // Dibujar HUD (vidas) si la fuente se cargó
        if (hudEnabled) {
            livesText.setString(std::string("Lives: ") + std::to_string(fish.getLives()) + std::string("/") + std::to_string(fish.getMaxLives()));
            window.draw(livesText);
        }
        window.display();
    }

    return 0;
}
