CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -IC:/Users/Ascencio/Documents/CETI/POO/SFML/include
LDFLAGS = -LC:/Users/Ascencio/Documents/CETI/POO/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system

SRC = $(wildcard src/*.cpp)
# Transforma la lista de archivos fuente (src/X.cpp) en archivos objeto (build/X.o)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))
OUT = build/game.exe

# Regla para asegurar que la carpeta 'build' exista
.PHONY: all clean
all: build $(OUT)

# Regla para crear la carpeta build si no existe
build:
	mkdir -p build

# ENLACE: Crea el ejecutable a partir de todos los archivos objeto
$(OUT): $(OBJ)
	$(CXX) $(OBJ) -o $(OUT) $(LDFLAGS)

# COMPILACIÓN: Regla genérica para transformar src/X.cpp en build/X.o
build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# LIMPIEZA: Borra todos los archivos generados
clean:
	@echo "Limpiando archivos de compilación..."
	# Usa @ para suprimir el comando y - para ignorar errores si la carpeta no existe
	-rmdir /s /q build