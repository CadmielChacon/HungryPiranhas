<!-- Copilot instructions tailored for the HungryPiranhas SFML C++ game -->
# Repo intent
This repository is a small SFML-based 2D game. The engine is a single-threaded game loop implemented in `src/main.cpp` that manages game objects implemented as small classes in `include/` and `src/` (e.g., `Fish`, `Piranha`, `Coral`, `Can`, `Bubble`, `LivesHUD`, `Heart`). AI agents should treat this as an event-driven rendering loop with explicit resource loading and simple collision checks.

# Big picture (what to know first)
- Main loop: `src/main.cpp` — initializes SFML window, loads assets from `assets/`, updates objects, handles collisions using `getGlobalBounds()` and `tryTakeDamage()` patterns, and draws sprites.
- Components: one class per file under `include/` with implementation in `src/` (e.g., `include/Piranha.hpp` / `src/Piranha.cpp`). Classes frequently provide `load()` or `loadTexture()` methods returning `bool` and may use `static` texture members for shared textures (see `Piranha`).
- Resource convention: assets are stored in `assets/` and loaded with relative paths from `src/main.cpp` (e.g. `"../assets/fish.png"`). When running the built binary, ensure the working directory makes those relative paths valid (the build target is `build/game.exe`).

# Build & run (developer workflows)
- Primary build options:
  - Use the included `makefile` (expects a POSIX-like shell or msys/mingw tooling). Output: `build/game.exe`.
  - VS Code build task: the workspace contains a `build-sfml` task that invokes `C:/msys64/mingw64/bin/g++.exe` with SFML include/lib paths. This mirrors the `makefile` flags.
- Important flags/paths: SFML headers/libs are referenced by absolute paths in the `makefile` and task (e.g. `C:/Users/Ascencio/Documents/CETI/POO/SFML/include`). Update these if your SFML installation differs.
- Run: after building, run `build/game.exe` from the repository root (or ensure the process cwd makes `../assets/...` resolve correctly). If images/fonts fail to load, `main.cpp` exits with an error code — follow stderr messages.

# Project-specific conventions & patterns
- File layout: `include/*.hpp` declare classes; `src/*.cpp` implement them. Use `#pragma once` (already present).
- Texture loading idiom: many classes expose `bool load(const std::string&)` or `static bool loadTexture(const std::string&)`. These return false on failure and the caller (usually `main.cpp`) treats failures as critical. Example: `Piranha::loadTexture("../assets/piranha.png")`.
- Shared resources: some sprites/textures are shared via `static` members (see `include/Piranha.hpp`). Avoid duplicating textures; prefer the class-provided loader if present.
- Timing: game objects use `sf::Clock` and delta-time is computed in `main.cpp` with `deltaClock.restart()` and passed into `update(deltaTime)` for frame-rate independent movement.
- Collision: AABB intersection via `sf::FloatRect` returned by `getGlobalBounds()`; collisions are handled in `main.cpp` with immediate damage logic and state resets.

# Integration points & external deps
- SFML (Graphics, Window, System) — makefile and VS Code task link `-lsfml-graphics -lsfml-window -lsfml-system`.
- Absolute include/lib paths are embedded in the `makefile` and VS Code task; update them when switching machines.
- Assets folder: `assets/` contains images and `pixelifysans.ttf`. Keep filenames and relative layout intact when modifying resources.

# When editing code — practical guidance for agents
- Preserve resource-loading patterns: if adding a new sprite class, implement `bool load(const std::string&)` or `static bool loadTexture(...)` and call it from `main.cpp` during initialization.
- Follow one-class-per-file in `include/` and `src/`. Use existing naming and method signatures (e.g., `update(float deltaTime)`, `draw(sf::RenderWindow&)`, `getGlobalBounds()`).
- Avoid changing relative asset paths in `main.cpp` unless you update run instructions and tests accordingly.
- Prefer using existing timers (`sf::Clock`) and deltaTime patterns instead of introducing new timing libraries.

# Debugging tips
- Check stderr printed by `main.cpp` — it logs asset-load failures and other runtime events.
- If the build fails due to SFML include/link paths, update the `makefile` or the workspace `build-sfml` task to point to your local SFML installation.
- To reproduce a failing texture load locally, run the binary from the `build/` folder and verify `../assets/...` exists relative to that cwd.

# Files to inspect for context
- `src/main.cpp` — centralized game loop and object orchestration.
- `include/Piranha.hpp`, `include/Fish.hpp`, `include/LivesHUD.hpp` — representative class patterns (static texture usage, load methods, update/draw API).
- `makefile` and the VS Code `build-sfml` task — how the project is compiled and linked.

# When you need clarification
- Ask which SFML installation/path the developer uses and whether they prefer running from `build/` or project root. Also ask if adding automated tests or CI is desired.

# Next step
- I will update the todo list and ask for feedback on unclear or missing parts.
