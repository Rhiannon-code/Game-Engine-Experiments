# GameEngine

A small 2D & 3D game engine and editor, written in C++20 as a learning + portfolio
project. Built around SDL2, OpenGL (4.6 on Linux/Windows, 4.1 on macOS), an
EnTT-based ECS, and a Dear ImGui editor. Runs on Linux, Windows, and macOS.

## Status

Bootstrap: SDL2 window + OpenGL 4.6 core context + Dear ImGui, drawing a
triangle. Subsystems land from here per the roadmap.

## Prerequisites

All you strictly need is a **C++20 compiler, CMake ≥ 3.20, Ninja, and Git**.
Every library (SDL2, glm, glad, Dear ImGui, doctest, and later EnTT/stb/ImGuizmo) is resolved automatically: 
if it's installed as a system package CMake
uses it, otherwise CMake fetches and builds it from source. No vcpkg, no manual
dependency wrangling.

### Linux (Arch Linux)

```bash
sudo pacman -S --needed base-devel cmake ninja git sdl2-compat glm doctest
paru -S --needed renderdoc   # optional: GPU frame debugger
```

Installing `sdl2-compat`, `glm`, and `doctest` keeps the configure step fast,
omit them and CMake fetches those from source instead.

### Windows

Install **Visual Studio 2022** ("Desktop development with C++" workload) or
MSYS2/MinGW, plus **CMake**, **Ninja**, and **Git** all available from the
Visual Studio installer, `winget`, or your package manager. No SDL2/glm install
is required: CMake fetches and *statically* links them, so there is no DLL to
copy next to the executable. Configure and build from a "Developer" shell:

```powershell
cmake -B build -G Ninja
cmake --build build
```

### macOS

```bash
xcode-select --install                 # Apple Clang + toolchain
brew install cmake ninja git           # sdl2/glm optional (else fetched)
```

macOS runs on an **OpenGL 4.1 core** context, Apple froze desktop OpenGL at 4.1
and deprecated it, while Linux and Windows use **4.6**. The engine picks the
right version per platform automatically (`GE_GL_*` in `src/main.cpp`,
`GAMEENGINE_GL_VERSION` in `CMakeLists.txt`), and features that require GL 4.3+
(e.g. the debug output callback) are compiled out there. As the renderer grows,
any GL 4.3+‑only feature (compute shaders, SSBOs, DSA) will need a 4.1 fallback
path, or eventually a separate backend, to stay macOS compatible.

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
11. **Lighting & materials:** Blinn-Phong (directional + point lights), stretch: PBR
12. **3D in the editor:** mesh/material/light components, 3D gizmos, scene grid

## Layout

```
src/      engine + editor source
tests/    doctest unit tests
```
