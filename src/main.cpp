// Tiempo total jugado (no aumenta en pausa)
// Actualizar y mostrar el tiempo jugado (usar totalGameTime)
// Actualizar y mostrar el tiempo jugado (usar totalGameTime)
// Actualizar y mostrar el tiempo jugado (usar totalGameTime)
// Mostrar tiempo final en game over (usar totalGameTime)
// Mostrar tiempo final en game over (usar totalGameTime)
// Mostrar tiempo final en game over (usar totalGameTime)
// Mostrar tiempo final en game over (usar totalGameTime)
#include <SFML/Graphics.hpp>
#include "../include/Fish.hpp"
#include "../include/Bubble.hpp" 
#include "../include/Piranha.hpp"
#include "../include/Coral.hpp"
#include "../include/Can.hpp"
#include "../include/LivesHUD.hpp"
#include "../include/Heart.hpp" 
#include <iostream>
#include <vector>
#include <random>
#include <iomanip> 
#include <sstream> 

sf::Clock gameClock;
int main() {
    sf::RenderWindow window(sf::VideoMode(1300, 700), "Hungry Piranhas");

    // ocultar el cursor
    window.setMouseCursorVisible(false);

    Fish fish;
    
    // verificacion critica
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
    // Crear columna de pirañas en el borde derecho 
    std::vector<Piranha> piranhas;
    const int pCount = 6;
    float marginRight = 60.0f;
    float startY = 80.0f;
    float spacing = 90.0f;
    float xPos = static_cast<float>(window.getSize().x) - marginRight;
    for (int i = 0; i < pCount; ++i) {
        piranhas.emplace_back(sf::Vector2f(xPos, startY + i * spacing));
    }

    // Dificultad del juego: variable maestra que aumenta lentamente
    float gameDifficulty = 1.0f; // empieza en 1.0 (normal)
    const float DIFFICULTY_INTERVAL = 10.0f; // cada 10 segundos
    const float DIFFICULTY_STEP = 0.1f; // incremento de +0.1 por intervalo
    const float MAX_DIFFICULTY = 3.0f; // límite máximo para evitar imposibilidad
    // Tiempo total jugado (no aumenta en pausa)
    float totalGameTime = 0.0f;
    float lastDifficultyTime = 0.0f;
    
    // Guardar los tiempos base de spawn para poder escalarlos
    float baseCoralSpawnInterval = 2.0f;
    float baseCanSpawnInterval = 1.5f;

    // Contenedor para corales
    std::vector<Coral> corals;
    sf::Clock coralSpawnTimer;
    float coralSpawnInterval = 2.0f;  // cada 2 segundos
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> coralTypeDist(0, 3);  // tipo de coral 
    std::uniform_real_distribution<> coralXDist(50.0f, 1250.0f);  // posicion X aleatoria
    float coralY = 630.0f;  // posicion Y fija

    // Contenedor para latas
    std::vector<Can> cans;
    sf::Clock canSpawnTimer;
    float canSpawnInterval = 1.5f;  // cada 1.5 segundos
    std::uniform_int_distribution<> canTypeDist(0, 2);  // tipo de lata 
    std::uniform_real_distribution<> canXDist(50.0f, 1250.0f);  // posicion X aleatoria

    // Cargar textura de corazon
    Heart heart;
    if (!heart.loadTexture("../assets/heart.png")) {
        std::cerr << "Error Crítico: Terminando el programa porque no se cargó 'heart.png'.\n";
        return 1;
    }

    sf::Clock heartSpawnTimer; // temporizador para controlar la aparicion del corazon

    // Cargar textura de pantalla de game over
    sf::Texture gameOverTexture;
    if (!gameOverTexture.loadFromFile("../assets/gomain.png")) {
        std::cerr << "Error Crítico: No se pudo cargar 'gomain.png'.\n";
        return 1;
    }
    sf::Sprite gameOverSprite(gameOverTexture);
    gameOverSprite.setScale(8.0f, 8.0f); // Escalar para que sea visible
    gameOverSprite.setPosition(
        (window.getSize().x - gameOverTexture.getSize().x * 8.0f) / 2.0f,
        (window.getSize().y - gameOverTexture.getSize().y * 8.0f) / 2.0f
    );

    // Cargar fuente PixelifySans
    sf::Font font;
    if (!font.loadFromFile("../assets/pixelifysans.ttf")) {
        std::cerr << "Error: No se pudo cargar la fuente 'pixelifysans.ttf'.\n";
        return 1;
    }

    sf::Text timerText;
    timerText.setFont(font);
    timerText.setCharacterSize(30);
    timerText.setFillColor(sf::Color::White);
    timerText.setStyle(sf::Text::Bold);

    // Texto para el menú (pausa/game over)
    sf::Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(50);
    statusText.setFillColor(sf::Color::White);
    statusText.setStyle(sf::Text::Bold);

    // Cargar textura y sprite para el boton de reinicio
    sf::Texture rebootTexture;
    if (!rebootTexture.loadFromFile("../assets/reboot.png")) {
        std::cerr << "Error Crítico: No se pudo cargar 'reboot.png'.\n";
        return 1;
    }
    sf::Sprite rebootSprite(rebootTexture);
    rebootSprite.setScale(8.0f, 8.0f); // Ajustar el tamaño del boton
    rebootSprite.setPosition(
        gameOverSprite.getPosition().x + 146.0f, // Posicion relativa a gomain.png
        gameOverSprite.getPosition().y + 232.0f
    );

    // Cargar textura y sprite para el boton de salida
    sf::Texture exitTexture;
    if (!exitTexture.loadFromFile("../assets/exit.png")) {
        std::cerr << "Error Crítico: No se pudo cargar 'exit.png'.\n";
        return 1;
    }
    sf::Sprite exitSprite(exitTexture);
    exitSprite.setScale(8.0f,8.0f); // Ajustar el tamaño del boton
    exitSprite.setPosition(
        gameOverSprite.getPosition().x + 260.0f, // Posicion relativa a gomain.png
        gameOverSprite.getPosition().y + 240.0f
    );

    // Crear sprite de resume (reutilizar reboot para pausa)
    sf::Sprite resumeSprite = rebootSprite; // Resume usa el mismo sprite que reboot para simplicidad

    // Estado de pausa y game over
    bool isPaused = false;
    bool isGameOver = false;

    // Inicialización completa
    sf::Clock deltaClock; // Crear un reloj separado para deltaTime
    while (window.isOpen()) {
        // Calcular el tiempo que paso desde el ultimo frame
        sf::Time dt = deltaClock.restart();
        float deltaTime = dt.asSeconds(); // DeltaTime en segundos
        // Acumular tiempo de juego solo cuando no está en pausa ni en game over
        if (!isPaused && !isGameOver) {
            totalGameTime += deltaTime;
        }
        
        // Manejo de eventos
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            // Detectar tecla Space para pausar/reanudar
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                if (!isGameOver) {
                    isPaused = !isPaused;
                }
            }
        }
        
        // Lógica - SOLO si no está pausado ni en game over
        if (!isPaused && !isGameOver) {
            fish.update(window); 
        }
        livesHUD.update(fish.getLives());

        // Actualizar temporizador en pantalla (esquina superior derecha, solo si no estamos en menu)
        if (!isPaused && !isGameOver) {
            int seconds = static_cast<int>(totalGameTime);
            timerText.setString(std::string("Tiempo: ") + std::to_string(seconds) + "s");
            float posX = window.getSize().x - timerText.getGlobalBounds().width - 20.0f;
            float posY = 20.0f;
            timerText.setPosition(posX, posY);
        }

        // Solo actualizar lógica de juego si no está pausado y no está en game over
        if (!isPaused && !isGameOver) {

        // Actualizar dificultad basada en totalGameTime cada DIFFICULTY_INTERVAL segundos
        if (totalGameTime - lastDifficultyTime >= DIFFICULTY_INTERVAL) {
            gameDifficulty += DIFFICULTY_STEP;
            if (gameDifficulty > MAX_DIFFICULTY) {
                gameDifficulty = MAX_DIFFICULTY;
            }
            lastDifficultyTime = totalGameTime;
            std::cerr << "Difficulty increased: " << gameDifficulty << " (max: " << MAX_DIFFICULTY << ")" << std::endl;
        }

        // Actualizar pirañas (aplicar dificultad antes de actualizar)
        for (size_t i = 0; i < piranhas.size(); ++i) {
            piranhas[i].applyDifficulty(gameDifficulty);
            piranhas[i].update(deltaTime);
        }

        // Colisiones con pirañas
        for (size_t i = 0; i < piranhas.size(); ++i) {
            if (fish.getHitbox().intersects(piranhas[i].getGlobalBounds()) && !fish.isHit()) {
                if (fish.tryTakeDamage(1)) {
                    fish.hit(); // Activar efecto de parpadeo e invulnerabilidad temporal
                    livesHUD.update(fish.getLives());
                    std::cerr << "Fish hit! Lives: " << fish.getLives() << std::endl;
                    if (fish.getLives() <= 0) {
                        std::cerr << "Fish died. Game over." << std::endl;
                        window.draw(gameOverSprite); // Dibujar la imagen de game over

                        // Mostrar el cursor antes de dibujar la pantalla de game over
                        window.setMouseCursorVisible(true);

                        // Actualizar y mostrar el tiempo jugado
                        sf::Time elapsed = gameClock.getElapsedTime();
                        // Convertir el tiempo transcurrido a formato mm:ss
                        int totalSeconds = static_cast<int>(elapsed.asSeconds());
                        int minutes = totalSeconds / 60;
                        int seconds = totalSeconds % 60;

                        // Corregir concatenación de cadenas con inicializacion explicita
                        std::string minutesStr = (minutes < 10 ? "0" : "") + std::to_string(minutes);
                        std::string secondsStr = (seconds < 10 ? "0" : "") + std::to_string(seconds);
                        std::string timeString = "Time: " + minutesStr + ":" + secondsStr;

                        timerText.setString(timeString);

                        // Depuracion: Imprimir el tiempo calculado
                        std::cerr << "Tiempo calculado: " << timeString << std::endl;

                        // Ajustar la posición del texto en la ventana 
                        float xPos = gameOverSprite.getPosition().x + 143.0f;
                        float yPos = gameOverSprite.getPosition().y + 185.0f;
                        timerText.setPosition(xPos, yPos);

                        window.draw(timerText);

                        // Dibujar el boton de reinicio
                        window.draw(rebootSprite);
                        // Dibujar el boton de salida
                        window.draw(exitSprite);
                        window.display();

                        bool restartGame = false;
                        while (!restartGame) {
                            sf::Event event;
                            while (window.pollEvent(event)) {
                                if (event.type == sf::Event::Closed) {
                                    window.close();
                                    return 0;
                                }

                                // Detectar clic en el boton de reinicio
                                if (event.type == sf::Event::MouseButtonPressed &&
                                    event.mouseButton.button == sf::Mouse::Left) {
                                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                                    if (rebootSprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                                        // Mostrar contador de reinicio durante 3 segundos
                                        sf::Clock countdownClock;
                                        while (countdownClock.getElapsedTime().asSeconds() < 3.0f) {
                                            window.clear();
                                            window.draw(backgroundSprite);
                                            
                                            float elapsedSeconds = countdownClock.getElapsedTime().asSeconds();
                                            int countdownValue = static_cast<int>(3 - elapsedSeconds);
                                            if (countdownValue < 1) countdownValue = 0;
                                            
                                            sf::Text countdownText;
                                            countdownText.setFont(font);
                                            countdownText.setCharacterSize(120);
                                            countdownText.setFillColor(sf::Color::White);
                                            countdownText.setStyle(sf::Text::Bold);
                                            countdownText.setString(std::to_string(countdownValue));
                                            
                                            // Centrar el contador en la pantalla
                                            sf::FloatRect textBounds = countdownText.getLocalBounds();
                                            countdownText.setPosition(
                                                (window.getSize().x - textBounds.width) / 2.0f,
                                                (window.getSize().y - textBounds.height) / 2.0f
                                            );
                                            
                                            window.draw(countdownText);
                                            window.display();
                                            
                                            // Manejar eventos para permitir cerrar la ventana
                                            sf::Event closeEvent;
                                            while (window.pollEvent(closeEvent)) {
                                                if (closeEvent.type == sf::Event::Closed) {
                                                    window.close();
                                                    return 0;
                                                }
                                            }
                                        }
                                        
                                        // Reiniciar el juego después del contador
                                        fish.reset();
                                        totalGameTime = 0.0f;
                                        lastDifficultyTime = 0.0f;
                                        gameDifficulty = 1.0f;
                                        corals.clear();
                                        cans.clear();
                                        bubbles.clear();
                                        heart.setIsVisible(false);
                                        window.setMouseCursorVisible(false);
                                        restartGame = true;
                                    }
                                    // Detectar clic en el boton de salida
                                    else if (exitSprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                                        window.close();
                                        return 0;
                                    }
                                }
                            }
                        }

                    }
                    break; // evitar multiples daños en la misma actualizacion
                }
            }
        }

        // Spawn aleatorio de corales con frecuencia escalada por dificultad
        coralSpawnInterval = baseCoralSpawnInterval / gameDifficulty;
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
            if (corals[i].isAlive() && fish.getHitbox().intersects(corals[i].getGlobalBounds()) && !fish.isHit()) {
                if (fish.tryTakeDamage(1)) {
                    fish.hit(); // Activar efecto de parpadeo e invulnerabilidad temporal
                    livesHUD.update(fish.getLives());
                    std::cerr << "Fish hit by coral! Lives: " << fish.getLives() << std::endl;
                    // Marcar coral como muerto para no causar daño otra vez
                    corals[i] = Coral(sf::Vector2f(-1000.0f, -1000.0f), 0);
                    if (fish.getLives() <= 0) {
                        std::cerr << "Fish died. Game over." << std::endl;
                        window.draw(gameOverSprite); // Dibujar la imagen de game over

                        // Mostrar el cursor antes de dibujar la pantalla de game over
                        window.setMouseCursorVisible(true);

                        // Actualizar y mostrar el tiempo jugado
                        sf::Time elapsed = gameClock.getElapsedTime();
                        // Convertir el tiempo transcurrido a formato mm:ss
                        int totalSeconds = static_cast<int>(elapsed.asSeconds());
                        int minutes = totalSeconds / 60;
                        int seconds = totalSeconds % 60;

                        // Corregir concatenacion de cadenas con inicializacion explicita
                        std::string minutesStr = (minutes < 10 ? "0" : "") + std::to_string(minutes);
                        std::string secondsStr = (seconds < 10 ? "0" : "") + std::to_string(seconds);
                        std::string timeString = "Time: " + minutesStr + ":" + secondsStr;

                        timerText.setString(timeString);

                        // Depuracion: Imprimir el tiempo calculado
                        std::cerr << "Tiempo calculado: " << timeString << std::endl;

                        // Ajustar la posicion del texto en la ventana 
                        float xPos = gameOverSprite.getPosition().x + 143.0f;
                        float yPos = gameOverSprite.getPosition().y + 185.0f;
                        timerText.setPosition(xPos, yPos);

                        window.draw(timerText);

                        // Dibujar el boton de reinicio
                        window.draw(rebootSprite);
                        // Dibujar el boton de salida
                        window.draw(exitSprite);
                        window.display();

                        bool restartGame = false;
                        while (!restartGame) {
                            sf::Event event;
                            while (window.pollEvent(event)) {
                                if (event.type == sf::Event::Closed) {
                                    window.close();
                                    return 0;
                                }

                                // Detectar clic en el boton de reinicio
                                if (event.type == sf::Event::MouseButtonPressed &&
                                    event.mouseButton.button == sf::Mouse::Left) {
                                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                                    if (rebootSprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                                        // Mostrar contador de reinicio durante 3 segundos
                                        sf::Clock countdownClock;
                                        while (countdownClock.getElapsedTime().asSeconds() < 3.0f) {
                                            window.clear();
                                            window.draw(backgroundSprite);
                                            
                                            float elapsedSeconds = countdownClock.getElapsedTime().asSeconds();
                                            int countdownValue = static_cast<int>(3 - elapsedSeconds);
                                            if (countdownValue < 1) countdownValue = 0;
                                            
                                            sf::Text countdownText;
                                            countdownText.setFont(font);
                                            countdownText.setCharacterSize(120);
                                            countdownText.setFillColor(sf::Color::White);
                                            countdownText.setStyle(sf::Text::Bold);
                                            countdownText.setString(std::to_string(countdownValue));
                                            
                                            // Centrar el contador en la pantalla
                                            sf::FloatRect textBounds = countdownText.getLocalBounds();
                                            countdownText.setPosition(
                                                (window.getSize().x - textBounds.width) / 2.0f,
                                                (window.getSize().y - textBounds.height) / 2.0f
                                            );
                                            
                                            window.draw(countdownText);
                                            window.display();
                                            
                                            // Manejar eventos para permitir cerrar la ventana
                                            sf::Event closeEvent;
                                            while (window.pollEvent(closeEvent)) {
                                                if (closeEvent.type == sf::Event::Closed) {
                                                    window.close();
                                                    return 0;
                                                }
                                            }
                                        }
                                        
                                        // Reiniciar el juego despues del contador
                                        fish.reset();
                                        totalGameTime = 0.0f;
                                        lastDifficultyTime = 0.0f;
                                        gameDifficulty = 1.0f;
                                        corals.clear();
                                        cans.clear();
                                        bubbles.clear();
                                        heart.setIsVisible(false);
                                        window.setMouseCursorVisible(false);
                                        restartGame = true;
                                    }
                                    // Detectar clic en el boton de salida
                                    else if (exitSprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                                        window.close();
                                        return 0;
                                    }
                                }
                            }
                        }

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

        // Spawn aleatorio de latas con frecuencia escalada por dificultad
        canSpawnInterval = baseCanSpawnInterval / gameDifficulty;
        if (canSpawnTimer.getElapsedTime().asSeconds() >= canSpawnInterval) {
            int type = canTypeDist(gen);
            float xPos = canXDist(gen);
            cans.emplace_back(sf::Vector2f(xPos, -30.0f), type);  // Spawn en la parte superior
            std::cerr << "Can spawned at X: " << xPos << " Type: " << type << "! Total cans: " << cans.size() << std::endl;
            canSpawnTimer.restart();
        }

        // Actualizar latas (con dificultad aplicada)
        for (size_t i = 0; i < cans.size(); ++i) {
            cans[i].update(deltaTime, gameDifficulty);
        }

        // Colisiones con latas
        for (size_t i = 0; i < cans.size(); ++i) {
            if (cans[i].getIsAlive() && fish.getHitbox().intersects(cans[i].getGlobalBounds()) && !fish.isHit()) {
                if (fish.tryTakeDamage(1)) {
                    fish.hit(); // Activar efecto de parpadeo e invulnerabilidad temporal
                    livesHUD.update(fish.getLives());
                    std::cerr << "Fish hit by can! Lives: " << fish.getLives() << std::endl;
                    // Marcar lata como muerta para no causar daño otra vez
                    cans[i].setIsAlive(false);
                    if (fish.getLives() <= 0) {
                        std::cerr << "Fish died. Game over." << std::endl;
                        window.draw(gameOverSprite); // Dibujar la imagen de game over

                        // Mostrar el cursor antes de dibujar la pantalla de game over
                        window.setMouseCursorVisible(true);

                        // Actualizar y mostrar el tiempo jugado
                        sf::Time elapsed = gameClock.getElapsedTime();
                        // Convertir el tiempo transcurrido a formato mm:ss
                        int totalSeconds = static_cast<int>(elapsed.asSeconds());
                        int minutes = totalSeconds / 60;
                        int seconds = totalSeconds % 60;

                        // Corregir concatenacion de cadenas con inicializacion explicita
                        std::string minutesStr = (minutes < 10 ? "0" : "") + std::to_string(minutes);
                        std::string secondsStr = (seconds < 10 ? "0" : "") + std::to_string(seconds);
                        std::string timeString = "Time: " + minutesStr + ":" + secondsStr;

                        timerText.setString(timeString);

                        // Depuracion: Imprimir el tiempo calculado
                        std::cerr << "Tiempo calculado: " << timeString << std::endl;

                        // Ajustar la posicion del texto en la ventana 
                        float xPos = gameOverSprite.getPosition().x + 143.0f;
                        float yPos = gameOverSprite.getPosition().y + 185.0f;
                        timerText.setPosition(xPos, yPos);

                        window.draw(timerText);

                        // Dibujar el boton de reinicio
                        window.draw(rebootSprite);
                        // Dibujar el boton de salida
                        window.draw(exitSprite);
                        window.display();

                        bool restartGame = false;
                        while (!restartGame) {
                            sf::Event event;
                            while (window.pollEvent(event)) {
                                if (event.type == sf::Event::Closed) {
                                    window.close();
                                    return 0;
                                }

                                // Detectar clic en el boton de reinicio
                                if (event.type == sf::Event::MouseButtonPressed &&
                                    event.mouseButton.button == sf::Mouse::Left) {
                                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                                    if (rebootSprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                                        // Mostrar contador de reinicio durante 3 segundos
                                        sf::Clock countdownClock;
                                        while (countdownClock.getElapsedTime().asSeconds() < 3.0f) {
                                            window.clear();
                                            window.draw(backgroundSprite);
                                            
                                            float elapsedSeconds = countdownClock.getElapsedTime().asSeconds();
                                            int countdownValue = static_cast<int>(3 - elapsedSeconds);
                                            if (countdownValue < 1) countdownValue = 0;
                                            
                                            sf::Text countdownText;
                                            countdownText.setFont(font);
                                            countdownText.setCharacterSize(120);
                                            countdownText.setFillColor(sf::Color::White);
                                            countdownText.setStyle(sf::Text::Bold);
                                            countdownText.setString(std::to_string(countdownValue));
                                            
                                            // Centrar el contador en la pantalla
                                            sf::FloatRect textBounds = countdownText.getLocalBounds();
                                            countdownText.setPosition(
                                                (window.getSize().x - textBounds.width) / 2.0f,
                                                (window.getSize().y - textBounds.height) / 2.0f
                                            );
                                            
                                            window.draw(countdownText);
                                            window.display();
                                            
                                            // Manejar eventos para permitir cerrar la ventana
                                            sf::Event closeEvent;
                                            while (window.pollEvent(closeEvent)) {
                                                if (closeEvent.type == sf::Event::Closed) {
                                                    window.close();
                                                    return 0;
                                                }
                                            }
                                        }
                                        
                                        // Reiniciar el juego despues del contador
                                        fish.reset();
                                        totalGameTime = 0.0f;
                                        lastDifficultyTime = 0.0f;
                                        gameDifficulty = 1.0f;
                                        corals.clear();
                                        cans.clear();
                                        bubbles.clear();
                                        heart.setIsVisible(false);
                                        window.setMouseCursorVisible(false);
                                        restartGame = true;
                                    }
                                    // Detectar clic en el botón de salida
                                    else if (exitSprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                                        window.close();
                                        return 0;
                                    }
                                }
                            }
                        }

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
            // Vida util: 2 segundos, Velocidad de subida: 150px/s
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

        // Manejar la aparicion del corazon
        if (!heart.getIsVisible() && heartSpawnTimer.getElapsedTime().asSeconds() >= 30.0f) {
            float xPos = static_cast<float>(rand() % (window.getSize().x - 50) + 25); // Posicion aleatoria en X
            float yPos = static_cast<float>(rand() % (window.getSize().y - 50) + 25); // Posicion aleatoria en Y
            heart.spawn(sf::Vector2f(xPos, yPos));
            heartSpawnTimer.restart();
        }

        // Actualizar el corazon (fuera del bloque de pausa para que siempre caiga)
        heart.update(deltaTime);
        
        // Eliminar corazon si sale de la pantalla
        if (heart.getIsVisible() && heart.getGlobalBounds().getPosition().y > window.getSize().y) {
            heart.setIsVisible(false);
        }

        // Detectar colisiones con el corazon
        if (heart.getIsVisible() && fish.getGlobalBounds().intersects(heart.getGlobalBounds())) {
            if (fish.getLives() < fish.getMaxLives()) {
                fish.tryTakeDamage(-1); // Ganar una vida
            }
            heart.setIsVisible(false); // Desaparecer el corazon
        }

        // Verificar si el pez perdió todas sus vidas
        if (fish.getLives() <= 0) {
            isGameOver = true;
            window.setMouseCursorVisible(true);
        }
        } // Fin de if (!isPaused && !isGameOver)

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

        // Temporizador en pantalla (sobre el HUD) - solo si no estamos en menu
        if (!isPaused && !isGameOver) {
            window.draw(timerText);
        }

        // Dibujar el corazon
        heart.draw(window);

        // Menú unificado: Pausa o Game Over
        if (isPaused && !isGameOver) {
            // UI Minimalista de Pausa: Solo el texto
            statusText.setString("PAUSA");
            statusText.setPosition(
                (window.getSize().x - statusText.getGlobalBounds().width) / 2.0f,
                window.getSize().y / 2.0f
            );
            window.draw(statusText);
        } else if (isGameOver) {
            // UI Completa de Game Over: Panel, botones y textos
            window.draw(gameOverSprite); // Dibujar fondo de menú
            
            statusText.setString("GAME OVER");
            statusText.setPosition(
                (window.getSize().x - statusText.getGlobalBounds().width) / 2.0f,
                gameOverSprite.getPosition().y + 50.0f
            );
            window.draw(statusText);
            
            // Mostrar tiempo final
            int totalSeconds = static_cast<int>(totalGameTime);
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;
            std::string minutesStr = (minutes < 10 ? "0" : "") + std::to_string(minutes);
            std::string secondsStr = (seconds < 10 ? "0" : "") + std::to_string(seconds);
            timerText.setString(std::string("Tiempo: ") + minutesStr + ":" + secondsStr);
            timerText.setCharacterSize(30);
            timerText.setPosition(
                (window.getSize().x - timerText.getGlobalBounds().width) / 2.0f,
                gameOverSprite.getPosition().y + 150.0f
            );
            window.draw(timerText);
            
            // Dibujar botones de reinicio y salida
            window.draw(rebootSprite);
            window.draw(exitSprite);
        }

        // Manejo de eventos en menús (pausa o game over) - clics de botones
        if (isPaused || isGameOver) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                if (isPaused && resumeSprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    // Resume: volver al juego
                    isPaused = false;
                } else if (isGameOver && rebootSprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    // Reiniciar juego
                    fish.reset();
                    totalGameTime = 0.0f;
                    lastDifficultyTime = 0.0f;
                    gameDifficulty = 1.0f;
                    corals.clear();
                    cans.clear();
                    bubbles.clear();
                    heart.setIsVisible(false);
                    isPaused = false;
                    isGameOver = false;
                    window.setMouseCursorVisible(false);
                } else if (isGameOver && exitSprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    window.close();
                    return 0;
                }
            }
        }

        window.display();
    }

    return 0;
}
