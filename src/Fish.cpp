#include "../include/Fish.hpp"
#include <iostream>
#include <cmath> // Necesario para std::sqrtf
#include <SFML/Graphics.hpp>

Fish::Fish() : lives(MAX_LIVES) {}

bool Fish::load(const std::string& path) {
    std::cout << "Cargando: " << path << "\n";
    if (!texture.loadFromFile(path)) {
        std::cout << "Error: no se pudo cargar la textura del pez\n";
        return false;
    }
    sprite.setTexture(texture);
    // Obtener el tamaño de la textura
    sf::Vector2u textureSize = texture.getSize();
    // Esto asegura que la rotacion y la posicion del sprite se midan desde su centro.
    sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
    sprite.setScale(2.0f, 2.0f);
    sprite.setPosition(200, 200);
    return true;
}

void Fish::draw(sf::RenderWindow& window) {
    // Aplicar efecto de parpadeo si está herido
    if (m_isHit) {
        // Verificar si el tiempo de daño ya pasó
        if (m_hitTimer.getElapsedTime().asSeconds() > HIT_DURATION) {
            m_isHit = false;
            sprite.setColor(sf::Color::White); // Restaurar color normal
        } else {
            // Parpadeo: alternar entre visible (opaco) y semitransparente
            int blinkCycle = static_cast<int>(m_hitTimer.getElapsedTime().asMilliseconds() / 100) % 2;
            if (blinkCycle == 0) {
                sprite.setColor(sf::Color::White); // Visible
            } else {
                sprite.setColor(sf::Color(255, 255, 255, 128)); // Semitransparente
            }
        }
    }
    
    window.draw(sprite);
}

sf::Vector2f Fish::getPosition() const {
    return sprite.getPosition();
}

void Fish::move(const sf::Vector2f& offset) {
    // Llama al metodo move del sprite de SFML
    sprite.move(offset); 
}

sf::Vector2f Fish::getTailPosition() const {
    // Obtener la posicion del centro del pez
    sf::Vector2f centerPos = sprite.getPosition();
    
    // Obtener el tamaño de la textura original (antes de escalar)
    sf::Vector2u textureSize = texture.getSize();
    float originalWidth = textureSize.x;
    float scale = sprite.getScale().x; // Ambas escalas deberían ser iguales
    
    // la cola esta a -width/2 de distancia
    float tailDistance = (originalWidth / 2.0f) * scale;
    
    // Obtener la rotacion del pez en radianes
    float rotationDegrees = sprite.getRotation();
    float rotationRadians = rotationDegrees * 3.14159265f / 180.0f;
    
    // Calcular el desplazamiento en la dirección opuesta a donde mira
    float offsetX = tailDistance * std::cos(rotationRadians);
    float offsetY = tailDistance * std::sin(rotationRadians);
    
    // Retornar la posicion de la cola
    return centerPos + sf::Vector2f(offsetX, offsetY);
}

void Fish::update(const sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f objetivo(mousePos.x, mousePos.y);
    
    // Aplicar clamping al objetivo para que no salga de la pantalla
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = texture.getSize();
    float fishWidth = textureSize.x * sprite.getScale().x;
    float fishHeight = textureSize.y * sprite.getScale().y;
    
    objetivo.x = std::max(fishWidth / 2.0f, std::min(objetivo.x, float(windowSize.x) - fishWidth / 2.0f));
    objetivo.y = std::max(fishHeight / 2.0f, std::min(objetivo.y, float(windowSize.y) - fishHeight / 2.0f));
    
    sf::Vector2f posPez = sprite.getPosition(); // Usamos sprite.getPosition() directamente
    sf::Vector2f direccion = objetivo - posPez;
    isMoving = false;

    float magnitud = std::sqrtf(direccion.x * direccion.x + direccion.y * direccion.y);
    distanceToTarget = magnitud; // Guardar la distancia para usarla en shouldEmitBubble()
    
    if (magnitud > 5.0f) {
        // Calcular el ángulo en radianes usando atan2(y, x)
        float angulo_radianes = std::atan2(direccion.y, direccion.x);
        
        // Convertir radianes a grados
        const float PI = 3.14159265f;
        float angulo_grados = angulo_radianes * 180.0f / PI;

        // Ajuste del ángulo para que el pez mire hacia el cursor
        float angulo_ajustado = angulo_grados + 180.0f;

        // Ajuste de offset para que el pez mire hacia el cursor
        sprite.setRotation(angulo_ajustado);
        
        // crea un vector de longitud 1
        sf::Vector2f direccion_normalizada = direccion / magnitud;

        // Movimiento final
        sf::Vector2f movimiento = direccion_normalizada * max_speed;
        
        isMoving = true; 

        // Aplica el movimiento
        sprite.move(movimiento);
    } else {
        // Si esta muy cerca, no se mueve
        }
}
    bool Fish::shouldEmitBubble() const {
        if (isMoving) {
            // Mapear distancia [0..500px] a ratio [0..1]
            // Distancias grandes = pez lejos = se mueve rapido = mas burbujas
            float maxDistance = 500.0f; // puedes ajustar este valor
            float speedRatio = distanceToTarget / maxDistance;
            if (speedRatio < 0.0f) speedRatio = 0.0f;
            if (speedRatio > 1.0f) speedRatio = 1.0f;
            // Cuando speedRatio = 1 (lejos) -> intervalo minimo (muchas burbujas)
            // Cuando speedRatio = 0 (cerca) -> intervalo maximo (pocas burbujas)
            float interval = minBubbleInterval + (1.0f - speedRatio) * (maxBubbleInterval - minBubbleInterval);
            return bubbleTimer.getElapsedTime().asSeconds() >= interval;
        }
        return false;
    }

    void Fish::restartBubbleTimer() {
        bubbleTimer.restart();
    }

    int Fish::getLives() const {
        return lives;
    }

    bool Fish::tryTakeDamage(int amount) {
        // comprobar cooldown
        if (damageClock.getElapsedTime().asSeconds() < damageCooldownSeconds) {
            return false;
        }
        lives -= amount;
        if (lives < 0) lives = 0;
        damageClock.restart();
        return true;
    }

    sf::FloatRect Fish::getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

    sf::FloatRect Fish::getHitbox() const {
        sf::FloatRect bounds = sprite.getGlobalBounds();
        // Reducir el hitbox al 70% del tamaño original (30% de reducción)
        float reductionFactor = 0.70f;
        float widthReduction = bounds.width * (1.0f - reductionFactor) / 2.0f;
        float heightReduction = bounds.height * (1.0f - reductionFactor) / 2.0f;
        
        // Centrar el hitbox reducido respecto al sprite
        sf::FloatRect hitbox(
            bounds.left + widthReduction,
            bounds.top + heightReduction,
            bounds.width * reductionFactor,
            bounds.height * reductionFactor
        );
        
        return hitbox;
    }

    int Fish::getMaxLives() const {
        return MAX_LIVES;
    }

    void Fish::hit() {
        // Si ya está herido, no activar el efecto nuevamente
        if (m_isHit) return;
        
        m_isHit = true;
        m_hitTimer.restart();
    }

    void Fish::reset() {
        lives = MAX_LIVES; // Reiniciar las vidas
        bubbleTimer.restart(); // Reiniciar el temporizador de burbujas
        damageClock.restart(); // Reiniciar el temporizador de daño
        m_isHit = false; // Resetear estado de daño
        sprite.setColor(sf::Color::White); // Restaurar color normal
        sprite.setPosition(200, 200); // Restaurar la posición inicial
        isMoving = false; // Detener el movimiento
    }
