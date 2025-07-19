# Jump Node Control – Real-Time 2D 4X Space Strategy Game

An early prototype for a real-time two-dimensional 4X space strategy game with a custom engine written in modern C++ (C++20), designed for large-scale galaxies with high-precision 64-bit coordinates. The game world (galaxy) consists of multiple star systems connected by jump nodes, enabling faster-than-light travel between them. Spacecraft can also jump within a single star system, without the need to use jump nodes. Basic combat is also implemented, featuring projectile, missile and beam weapons.

## Features

- **Large World Simulation:** Double-precision coordinates for seamless navigation across vast, realistic distances.
- **Entity-Component-System (ECS):** Modular, extensible architecture for most game objects and behaviors.
- **Hierarchical Order System:** Entities execute complex, nested orders (move, attack, construct, colonize, etc.) with suborders and state tracking.
- **Pathfinding:** Dijkstra’s algorithm for efficient interstellar route planning.
- **Physics & Collision:** Robust vector/matrix math, polygon and circle collision, projectile/missile/beam logic.
- **DirectX 11 Rendering:** High-performance 2D rendering with lighting, effects, and animation support.
- **ImGui Integration:** In-game GUI for debugging, selection, and information display.
- **Data-Driven Content:** Engine settings and star names are defined in JSON for easy modding and extension.
- **Entity Templates:** Add new ship types or weapons via entity templates.

### Prerequisites

- Visual Studio 2022
- Windows 10/11
- DirectX 11 SDK
- [Boost](https://www.boost.org/)
- [FMT](https://fmt.dev/)
- [RapidJSON](https://rapidjson.org/)
- [ImGui](https://github.com/ocornut/imgui)
- [DirectXTK](https://github.com/microsoft/DirectXTK)

### Building

1. Clone the repository.
2. Open `DirectX_Game.vcxproj` in Visual Studio 2022.
3. Ensure all library dependencies are available and paths are set in project properties.
4. Build the solution.

### Running

- Run the built executable. The game will initialize a console window for debug output and the main game window.
- Use mouse and keyboard to interact with the game world.

## Project Structure

| File             | Description                                                      |
|------------------|------------------------------------------------------------------|
| `Main.cpp`       | Entry point, main loop, initialization.                          |
| `Engine.h`       | Rendering, DirectX objects, global engine settings.              |
| `ECS.h`          | Entity-Component-System core and component definitions.          |
| `Classes.h`      | Orders, templates, and supporting gameplay classes.              |
| `Functions.h`    | Order execution, pathfinding, and entity logic.                  |
| `Vector2.h`      | 2D vector math and geometry utilities.                           |
| `Matrix.h`       | 3x3 matrix math for transformations.                             |
| `Rect.h`         | Rectangle and bounding box utilities.                            |
| `Utilities.h`    | Timing, random numbers, math helpers, assertions.                |
| `settings.json`  | Game configuration.                                              |
| `stars.json`     | Star and constellation names.                                    |

## License

All content and source code for this game are subject to the terms of the MIT License.