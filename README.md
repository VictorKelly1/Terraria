# 🛠️ Terraria Clone Engine (Raylib + OpenGL)
A 2D sandbox game inspired by Terraria, built from scratch in C++ utilizing Raylib and OpenGL. 

🚀 Features
High-Performance Rendering: Efficient texture management via a custom Texture Atlas system designed for optimized block drawing.

World Generation: Procedural algorithms for creating and structuring dynamic 2D grid maps, tracking live block states (Air, Dirt, etc.).

Graphical User Interface (GUI): Seamless integration of Dear ImGui (via rlImGui) for real-time debugging tools, telemetry, and map editing.

Physics: Grid-based collision handling, precise screen-to-world coordinate calculation via mouse input (GetScreenToWorld2D), and indexed grid rounding using std::floor.

## 🛠️ Tech Stack

| Component | Technology | Purpose |
| :--- | :--- | :--- |
| **Language** | C++17 | Core logic and high-performance execution. |
| **Graphics** | OpenGL & Raylib | Window management, camera controls, and texture rendering. |
| **UI Tools** | Dear ImGui (`rlImGui`) | Development overlays and real-time FPS monitoring. |
| **Build System** | CMake | Cross-platform build automation. |
| **Recommended IDE** | Neovim + `clangd` | Seamless indexing powered by `compile_commands.json`. |
