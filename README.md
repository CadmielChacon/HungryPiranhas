## 📝 Descripción del Proyecto

**Hungry Piranhas** es un juego en 2D desarrollado en C++ con SFML, donde controlas a un pez que debe sobrevivir el mayor tiempo posible en un océano lleno de peligros. El jugador deberá esquivar pirañas, corales, latas contaminantes y mantenerse siempre en movimiento para evitar quedar atrapado. El desafío aumenta conforme más enemigos aparecen y la velocidad de los obstáculos se incrementa.

### 🎯 Objetivo del Juego

Mantener al pez con vida el mayor tiempo posible esquivando:
- **Pirañas** que se mueven lateralmente
- **Corales venenosos** que emergen desde el fondo
- **Latas contaminantes** que caen desde la superficie
- **Administrar tus vidas** mientras generas burbujas como rastro al moverte
Tu puntaje se basa en cuánto tiempo sobrevives.

### 🎮 Controles


- **Mouse:** controla completamente la posición del pez
- **Tecla Espacio:** pausa la partida / regresa a la partida
- **(Opcional)** Click izquierdo para interactuar en futuros menús o modos
Es un control intuitivo y accesible para todo tipo de jugador.

### ⚙️ Mecánicas

- **Movimiento fluido:** el pez sigue al cursor con suavidad y genera un rastro dinámico de burbujas.
- **Colisiones y daño:** cada vez que tocas un enemigo, pierdes una vida.
- **Sistema de vidas animado:** los íconos de vidas se muestran usando un spritesheet (10 estados posibles).

- **Spawners dinámicos:**
  * Las pirañas aparecen al costado derecho.
  * Las latas caen desde arriba.
  * Los corales emergen desde el fondo de manera aleatoria.

- **Efectos visuales:**
  * Burbujas que suben, se desvanecen y desaparecen suavemente.
  * Parpadeo en el pez por colicion.
  * Pantalla de inicio animada con una persecucion.
  * Fondo oceánico a pantalla completa.

### 🏆 Características

- Movimiento suave del personaje usando interpolación 
- Burbujas dinámicas dependientes de distancia 
- Sistema de colisiones preciso con sprites 
- Pirañas, corales y latas con físicas independientes 
- Sistema de vidas con spritesheet (10 estados) 
- Fondo animado o estático escalado correctamente 
- Sistema de spawn con tiempos variables
- Arquitectura modular: `Fish`, `Bubble`, `Piranha`, `Coral`, `Can`, `LivesHUD` 
- Proyecto compilable con Makefile y compatible con *launch.json* 

### 👥 Equipo

- **Líder**: Cadmiel Yareli Chacon Camacho (CadmielChacon)
- **Integrante 2**: Yael Abdiel Gonzalez Martinez (Abdiel367)

### 🛠️ Tecnologías

- **Lenguaje:** C++
- **Librería Gráfica:** SFML 2.6
- **IDE:** Visual Studio Code
- **Compilación:** MinGW (g++) + Makefile
- **Control de Versiones:** Git / GitHub
- **Edición de arte:** PixilArt, Piskel.
- **Fuentes:** Google Fonts (Pixelify Sans)

### 📜 Créditos


- **Fuente pixel-art:**
  [https://fonts.google.com/specimen/Pixelify+Sans](https://fonts.google.com/specimen/Pixelify+Sans)
- **Arte del juego:**
  Creado en [https://www.pixilart.com](https://www.pixilart.com)
- **Música ambiental:**
  Proporcionada por Pixabay.com
- **Inspiración del gameplay:**
  *Ataque de Pirañas* del juego *2 Player Games – The Challenge*
- **Agradecimientos personales:**
  A las herramientas, a mi compañero por su esfuerzo…
  y al gato que no me dejó dormir pero al final ayudó a terminar el proyecto. 

___
