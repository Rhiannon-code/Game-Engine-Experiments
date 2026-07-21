# GameEngine

A small 2D & 3D game engine and editor, written in C++20 as a learning + portfolio
project. Built around SDL2, OpenGL 4.6, an EnTT-based ECS, and a Dear ImGui
editor.

## Status

Bootstrap: SDL2 window + OpenGL 4.6 core context + Dear ImGui, drawing a
triangle. Subsystems land from here per the roadmap.

## Prerequisites (Arch / CachyOS)

System packages:

```bash
sudo pacman -S --needed base-devel cmake ninja git sdl2-compat glm yaml-cpp doctest
paru -S --needed glad renderdoc   # glad generator + GPU frame debugger
```

`glm`, `SDL2`, and `doctest` are found via `pacman`; `glad`, Dear ImGui (and
later EnTT / stb / ImGuizmo) are fetched and pinned by CMake `FetchContent`.

## Build & run

```bash
cmake -B build -G Ninja
cmake --build build
./build/engine
```

Debug builds enable AddressSanitizer + UBSan by default
(`-DGAMEENGINE_ENABLE_ASAN=OFF` to disable).

## Test

```bash
ctest --test-dir build --output-on-failure
```

## Roadmap

1. **Foundation:** window, input, GL context, render loop *(done)*
2. **2D renderer:** orthographic camera, batched sprite renderer, shaders/textures
3. **ECS + scene:** EnTT, Transform/SpriteRenderer/Tag components, render system
4. **Editor shell:** ImGui docking, scene rendered to a viewport panel
5. **Editor panels:** scene hierarchy, inspector, ImGuizmo transforms
6. **Serialization:** save/load scenes (YAML), asset folder
7. **Demo game:** a small complete game built entirely in the editor
8. **Polish:** README media, devblog, cleanup
9.  **3D foundations:** perspective camera, depth testing, editor fly/orbit camera
10. **Meshes & model loading:** Mesh/Model classes, load textured models (tinyobj/glTF)
11. **Lighting & materials:** Blinn-Phong (directional + point lights); stretch: PBR
12. **3D in the editor:** mesh/material/light components, 3D gizmos, scene grid

## Layout

```
src/      engine + editor source
tests/    doctest unit tests
```
