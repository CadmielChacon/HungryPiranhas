// Tiempo total jugado (no aumenta en pausa)
// Actualizar y mostrar el tiempo jugado (usar totalGameTime)
// Actualizar y mostrar el tiempo jugado (usar totalGameTime)
// Actualizar y mostrar el tiempo jugado (usar totalGameTime)
// Mostrar tiempo final en game over (usar totalGameTime)
// Mostrar tiempo final en game over (usar totalGameTime)
// Mostrar tiempo final en game over (usar totalGameTime)
// Mostrar tiempo final en game over (usar totalGameTime)
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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
#include <cstdlib>
#include <ctime>

sf::Clock gameClock;
// Estados del juego (Menu / Play / GameOver)
enum GameState { MENU, PLAY, GAMEOVER };
GameState currentState = MENU; // Arranca en MENU

int main() {
    sf::RenderWindow window(sf::VideoMode(1300, 700), "Hungry Piranhas");

    // Mostrar el cursor al inicio (el juego arranca en MENU)
    window.setMouseCursorVisible(true);

    // Semilla para rand() usado en la cinemática del menú
    std::srand(static_cast<unsigned>(std::time(nullptr)));

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

    // Assets del menú principal
    sf::Texture seaMenuTexture;
    if (!seaMenuTexture.loadFromFile("../assets/sea.png")) {
        std::cerr << "Aviso: no se pudo cargar 'sea.png' para el menú." << std::endl;
    }
    sf::Sprite seaSprite(seaMenuTexture);
    if (seaMenuTexture.getSize().x > 0 && seaMenuTexture.getSize().y > 0) {
        float scaleX = static_cast<float>(window.getSize().x) / seaMenuTexture.getSize().x;
        float scaleY = static_cast<float>(window.getSize().y) / seaMenuTexture.getSize().y;
        seaSprite.setScale(scaleX, scaleY);
    } else {
        seaSprite.setScale(1.0f, 1.0f);
    }
    seaSprite.setPosition(0.0f, 0.0f);

    sf::Texture menuTexture;
    if (!menuTexture.loadFromFile("../assets/menu.png")) {
        std::cerr << "Aviso: no se pudo cargar 'menu.png' para el menú." << std::endl;
    }
    sf::Sprite menuSprite(menuTexture);
    menuSprite.setScale(8.0f, 8.0f);
    // Centrar usando el tamaño global (ya incluye la escala)
    sf::FloatRect menuBounds = menuSprite.getGlobalBounds();
    menuSprite.setPosition(
        (window.getSize().x - menuBounds.width) / 2.0f,
        (window.getSize().y - menuBounds.height) / 2.0f
    );

    // Rectángulo del botón de SALIR (pixel art coords * escala 8)
    sf::FloatRect exitButtonRect(73 * 8.0f, 63 * 8.0f, 13 * 8.0f, 10 * 8.0f);
    
    // Rectángulo del botón de PLAY (pixel art coords * escala 8)
    sf::FloatRect playButtonRect(73 * 8.0f, 30 * 8.0f, 13 * 8.0f, 10 * 8.0f);
    
    // Rectángulo del botón de MUTE (pixel art coords * escala 8)
    sf::FloatRect muteButtonRect(73 * 8.0f, 47 * 8.0f, 13 * 8.0f, 10 * 8.0f);
    bool isMuted = false; // Estado de mute

    // ===== ASSETS CINEMÁTICOS DEL MENÚ =====
    // Capa translúcida
    sf::Texture translucentTexture;
    if (!translucentTexture.loadFromFile("../assets/traslucido.png")) {
        std::cerr << "Aviso: no se pudo cargar 'traslucido.png' para el menú." << std::endl;
    }
    sf::Sprite translucentSprite(translucentTexture);
    translucentSprite.setScale(8.0f, 8.0f);
    sf::FloatRect translucentBounds = translucentSprite.getGlobalBounds();
    translucentSprite.setPosition(
        (window.getSize().x - translucentBounds.width) / 2.0f,
        (window.getSize().y - translucentBounds.height) / 2.0f
    );
    // Aplicar transparencia (50% de opacidad)
    translucentSprite.setColor(sf::Color(255, 255, 255, 200));

    // Corales estáticos del menú - Arrecife denso con 12 corales variados
    std::vector<sf::Sprite> menuCorals;
    std::vector<std::string> coralAssets = {
        "../assets/coralR1.png",
        "../assets/coralR2.png",
        "../assets/coralP1.png",
        "../assets/coralP2.png"
    };
    std::vector<sf::Texture> coralTextures;
    for (const auto& assetPath : coralAssets) {
        sf::Texture coralTex;
        if (!coralTex.loadFromFile(assetPath)) {
            std::cerr << "Aviso: no se pudo cargar '" << assetPath << "' para el menú." << std::endl;
        }
        coralTextures.push_back(coralTex);
    }

    // Generar 12 corales distribuidos aleatoriamente a lo largo del ancho
    std::random_device rdCoral;
    std::mt19937 genCoral(rdCoral());
    std::uniform_int_distribution<> menuCoralTypeDist(0, 3); // 4 tipos de coral
    std::uniform_real_distribution<> menuCoralXDist(30.0f, static_cast<float>(window.getSize().x) - 30.0f);
    std::uniform_real_distribution<> menuCoralYDist(600.0f, 650.0f);
    
    const int numMenuCorals = 12;
    for (int i = 0; i < numMenuCorals; ++i) {
        int coralType = menuCoralTypeDist(genCoral);
        float coralX = menuCoralXDist(genCoral);
        float coralY = menuCoralYDist(genCoral);
        
        sf::Sprite coralSprite(coralTextures[coralType]);
        coralSprite.setScale(8.0f, 8.0f);
        coralSprite.setPosition(coralX, coralY);
        menuCorals.push_back(coralSprite);
    }

    // Actores cinemáticos del menú (Texturas direccionales)
    // Cargar pez - dirección izquierda (normal)
    sf::Texture menuFishTexture;
    if (!menuFishTexture.loadFromFile("../assets/fish.png")) {
        std::cerr << "Aviso: no se pudo cargar 'fish.png' para el menú cinemático." << std::endl;
    }
    // Cargar pez - dirección derecha (espejeado)
    sf::Texture menuFishTexture2;
    if (!menuFishTexture2.loadFromFile("../assets/fish2.png")) {
        std::cerr << "Aviso: no se pudo cargar 'fish2.png' para el menú cinemático." << std::endl;
    }
    sf::Sprite menuFishSprite(menuFishTexture);
    // Posición inicial basada en el tamaño de ventana; la piraña estará detrás del pez
    float initFishX = static_cast<float>(window.getSize().x) * 0.6f;
    float initFishY = static_cast<float>(window.getSize().y) * 0.35f;
    menuFishSprite.setPosition(initFishX, initFishY);

    // Cargar piraña - dirección izquierda (normal)
    sf::Texture menuPiranhaTexture;
    if (!menuPiranhaTexture.loadFromFile("../assets/piranha.png")) {
        std::cerr << "Aviso: no se pudo cargar 'piranha.png' para el menú cinemático." << std::endl;
    }
    // Cargar piraña - dirección derecha (espejeada)
    sf::Texture menuPiranhaTexture2;
    if (!menuPiranhaTexture2.loadFromFile("../assets/piranha2.png")) {
        std::cerr << "Aviso: no se pudo cargar 'piranha2.png' para el menú cinemático." << std::endl;
    }
    sf::Sprite menuPiranhaSprite(menuPiranhaTexture);
    // Colocar la piraña justo detrás del pez (misma altura Y)
    menuPiranhaSprite.setPosition(initFishX + 80.0f, initFishY);

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

    // Dificultad del juego: variable maestra que progresa de 1.0 a 3.0 en 10 minutos
    float gameDifficulty = 1.0f; // empieza en 1.0 (normal), termina en 3.0 a los 600s
    float totalGameTime = 0.0f; // Tiempo total jugado (no aumenta en pausa)
    
    // Guardar los tiempos base de spawn para poder escalarlos
    float basePiranhaSpawnInterval = 1.5f; // Base para cálculo de spawn agresivo
    float baseCoralSpawnInterval = 3.0f;  // Base para cálculo de spawn agresivo
    float baseCanSpawnInterval = 3.0f;    // Base para cálculo de spawn agresivo

    // ===== LÓGICA CINEMÁTICA DEL MENÚ =====
    bool menuMovingLeft = true;
    float menuFishSpeed = 0.5f;
    float menuPiranhaSpeed = 0.5f;

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

    // Cargar música de fondo
    sf::Music bgMusic;
    if (!bgMusic.openFromFile("../assets/Piranhas.mp3")) {
        std::cerr << "Aviso: no se pudo cargar la música de fondo 'Piranhas.mp3'.\n";
    } else {
        bgMusic.setLoop(true);
        bgMusic.setVolume(100);
        bgMusic.play(); // Iniciar la música
        std::cerr << "Música de fondo cargada y reproducida.\n";
    }

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
            // Detectar clic en MENU (botón SALIR y botón PLAY)
            if (currentState == MENU && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (exitButtonRect.contains(mousePos)) {
                    window.close();
                }
                // Detectar clic en botón PLAY
                if (playButtonRect.contains(mousePos)) {
                    // Mostrar contador de 3 segundos como en el Reboot
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
                    
                    // Después del countdown, cambiar a PLAY y reiniciar todo
                    currentState = PLAY;
                    window.setMouseCursorVisible(false); // Ocultar cursor durante el juego
                    gameClock.restart(); // Reiniciar el reloj del juego
                    coralSpawnTimer.restart();
                    canSpawnTimer.restart();
                    heartSpawnTimer.restart();
                    totalGameTime = 0.0f; // Reiniciar el contador de tiempo
                    std::cerr << "Transitioning to PLAY state after countdown. Cursor hidden.\n";
                }
                // Detectar clic en botón MUTE
                if (muteButtonRect.contains(mousePos)) {
                    isMuted = !isMuted; // Alternar estado
                    bgMusic.setVolume(isMuted ? 0 : 100); // Aplicar cambio de volumen
                    std::cerr << "Music muted: " << (isMuted ? "ON" : "OFF") << "\n";
                }
            }

            // Detectar clic en GAMEOVER (botones reiniciar / salir)
            if (currentState == GAMEOVER && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                // Si el jugador hace clic en el boton de salida, cerrar la ventana
                if (exitSprite.getGlobalBounds().contains(mousePos)) {
                    window.close();
                }
            }
        }
        
        // Lógica - SOLO si no está pausado ni en game over
        if (currentState == PLAY) {
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

        // Calcular dificultad: progresa linealmente de 1.0 a 3.0 en 300 segundos (5 minutos DEMO)
        float progress = std::min(totalGameTime / 300.0f, 1.0f);
        gameDifficulty = 1.0f + (progress * 2.0f);
        
        // Log de dificultad cada segundo
        static float lastLogTime = 0.0f;
        if (totalGameTime - lastLogTime >= 1.0f) {
            std::cerr << "Game Time: " << totalGameTime << "s, Difficulty: " << gameDifficulty << std::endl;
            lastLogTime = totalGameTime;
        }

        // Actualizar pirañas (aplicar dificultad antes de actualizar)
        for (size_t i = 0; i < piranhas.size(); ++i) {
            piranhas[i].applyDifficulty(gameDifficulty);
            piranhas[i].update(deltaTime);
        }

        // Control de activación de pirañas desde el game loop
        // Nueva fórmula solicitada: piranhaInterval = 1.8f / (gameDifficulty * 1.8f)
        // (equivalente a 1.0f / gameDifficulty, pero aplicamos exactamente la expresión)
        static float piranhaAccumulator = 0.0f;
        float piranhaInterval = 1.8f / (gameDifficulty * 1.8f);
        piranhaAccumulator += deltaTime;
        while (piranhaAccumulator >= piranhaInterval) {
            // Elegir una piraña aleatoria para activar
            std::uniform_int_distribution<> pIndexDist(0, static_cast<int>(piranhas.size() - 1));
            int idx = pIndexDist(gen);
            piranhas[idx].triggerAttackNow();
            piranhaAccumulator -= piranhaInterval;
            std::cerr << "Piranha triggered at index " << idx << " | Difficulty: " << gameDifficulty << "\n";
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
                                        gameDifficulty = 1.0f;
                                        corals.clear();
                                        cans.clear();
                                        bubbles.clear();
                                        heart.setIsVisible(false);
                                        currentState = PLAY; // Cambiar directamente a PLAY
                                        window.setMouseCursorVisible(false); // Ocultar cursor durante el juego
                                        gameClock.restart(); // Reiniciar el reloj del juego
                                        coralSpawnTimer.restart();
                                        canSpawnTimer.restart();
                                        heartSpawnTimer.restart();
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

        // Spawn aleatorio de corales con frecuencia escalada agresivamente por dificultad
        coralSpawnInterval = baseCoralSpawnInterval / (gameDifficulty * 1.2f);
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
                                        
                                        gameDifficulty = 1.0f;
                                        corals.clear();
                                        cans.clear();
                                        bubbles.clear();
                                        heart.setIsVisible(false);
                                        currentState = PLAY; // Cambiar directamente a PLAY
                                        window.setMouseCursorVisible(false); // Ocultar cursor durante el juego
                                        gameClock.restart(); // Reiniciar el reloj del juego
                                        coralSpawnTimer.restart();
                                        canSpawnTimer.restart();
                                        heartSpawnTimer.restart();
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

        // Spawn de latas con ráfagas: base aleatoria entre 0.3 y 2.3 segundos, dividida por la dificultad
        static float canSpawnAccumulator = 0.0f;
        float baseInterval = (rand() % 200 + 30) / 100.0f; // [0.30, 2.30]
        float timeToNextSpawn = baseInterval / gameDifficulty;

        canSpawnAccumulator += deltaTime;

        while (canSpawnAccumulator >= timeToNextSpawn) {
            int type = canTypeDist(gen);
            float xPos = canXDist(gen);
            cans.emplace_back(sf::Vector2f(xPos, -30.0f), type);
            std::cerr << "CAN SPAWN: Type " << type << " at X:" << xPos
                      << " | Difficulty: " << gameDifficulty
                      << " | Interval: " << timeToNextSpawn << "s" << std::endl;

            canSpawnAccumulator -= timeToNextSpawn;
            baseInterval = (rand() % 200 + 30) / 100.0f;
            timeToNextSpawn = baseInterval / gameDifficulty;
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
                                        
                                        gameDifficulty = 1.0f;
                                        corals.clear();
                                        cans.clear();
                                        bubbles.clear();
                                        heart.setIsVisible(false);
                                        currentState = PLAY; // Cambiar directamente a PLAY
                                        window.setMouseCursorVisible(false); // Ocultar cursor durante el juego
                                        gameClock.restart(); // Reiniciar el reloj del juego
                                        coralSpawnTimer.restart();
                                        canSpawnTimer.restart();
                                        heartSpawnTimer.restart();
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
        } // Fin de if (currentState == PLAY)

        // ===== LÓGICA DE ACTUALIZACIÓN DEL MENÚ =====
        if (currentState == MENU) {
            // Movimiento cinemático: mover SOLO al pez, anclar la piraña detrás (parenting)
            if (menuMovingLeft) {
                // Movimiento hacia la izquierda
                menuFishSprite.move(-menuFishSpeed, 0.0f);
                // Texturas para mirar a la izquierda
                menuFishSprite.setTexture(menuFishTexture);
                menuPiranhaSprite.setTexture(menuPiranhaTexture);
                // Anclar piraña detrás a la derecha del pez
                sf::Vector2f fishPos = menuFishSprite.getPosition();
                menuPiranhaSprite.setPosition(fishPos.x + 120.0f, fishPos.y);
            } else {
                // Movimiento hacia la derecha
                menuFishSprite.move(menuFishSpeed, 0.0f);
                // Texturas espejeadas para mirar a la derecha
                menuFishSprite.setTexture(menuFishTexture2);
                menuPiranhaSprite.setTexture(menuPiranhaTexture2);
                // Anclar piraña detrás a la izquierda del pez
                sf::Vector2f fishPos = menuFishSprite.getPosition();
                menuPiranhaSprite.setPosition(fishPos.x - 120.0f, fishPos.y);
            }

            // Verificar si ambos han salido completamente de la pantalla
            sf::FloatRect fishBounds = menuFishSprite.getGlobalBounds();
            sf::FloatRect piranhaGlobalBounds = menuPiranhaSprite.getGlobalBounds();
            
            bool fishOffScreen = (menuMovingLeft && fishBounds.left + fishBounds.width < 0.0f) ||
                                 (!menuMovingLeft && fishBounds.left > static_cast<float>(window.getSize().x));
            bool piranhaOffScreen = (menuMovingLeft && piranhaGlobalBounds.left + piranhaGlobalBounds.width < 0.0f) ||
                                    (!menuMovingLeft && piranhaGlobalBounds.left > static_cast<float>(window.getSize().x));

            if (fishOffScreen && piranhaOffScreen) {
                // Invertir dirección
                menuMovingLeft = !menuMovingLeft;

                // Reposicionar actores en el extremo opuesto
                // Generar Y aleatoria usando rand() para variedad (evitar bordes)
                float randomY = static_cast<float>(std::rand() % (window.getSize().y - 200) + 100);

                if (menuMovingLeft) {
                    // Reaparecen por la derecha; la piraña queda detrás (a la derecha del pez)
                    float fishX = static_cast<float>(window.getSize().x) + 50.0f;
                    menuFishSprite.setPosition(fishX, randomY);
                    menuPiranhaSprite.setPosition(fishX + 80.0f, randomY);
                } else {
                    // Reaparecen por la izquierda; la piraña queda detrás (a la izquierda del pez)
                    float fishX = -100.0f;
                    menuFishSprite.setPosition(fishX, randomY);
                    menuPiranhaSprite.setPosition(fishX - 80.0f, randomY);
                }
            }
        }

        // Dibujo
        if (currentState == MENU) {
            window.clear();
            // Orden de capas del MENÚ:
            window.draw(seaSprite);           // Fondo
            
            // Dibujar corales decorativos
            for (size_t i = 0; i < menuCorals.size(); ++i) {
                window.draw(menuCorals[i]);
            }
            
            window.draw(menuPiranhaSprite);   // Piraña detrás del pez
            window.draw(menuFishSprite);      // Pez
            window.draw(translucentSprite);   // Capa translúcida (efecto visual)
            window.draw(menuSprite);          // Texto y botones (frente)
        } else {
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
