cmake_minimum_required(VERSION 3.20)
project(GameEngine LANGUAGES C CXX)  # C is needed for glad's generated loader

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # feeds clang-tidy / editor tooling

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

option(GAMEENGINE_ENABLE_ASAN "Enable Address+UB sanitizers on Debug builds" ON)
option(GAMEENGINE_BUILD_TESTS "Build unit tests" ON)

include(FetchContent)

# ---------------------------------------------------------------------------
# Dependencies (Linux / Windows / macOS)
#
# System packages are preferred so the configure step stays fast where they're
# installed (Arch/CachyOS via pacman, macOS via Homebrew). When a package is
# missing — e.g. a fresh Windows checkout with no package manager — it is
# fetched and built from source instead, so `cmake -B build` just works on
# every platform with no manual dependency install and no vcpkg.
# ---------------------------------------------------------------------------

# --- SDL2: window + input --------------------------------------------------
find_package(SDL2 QUIET)
if(NOT TARGET SDL2::SDL2)
    message(STATUS "SDL2 not found on system; fetching from source")
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)  # static: no DLL to ship on Windows
    set(SDL_STATIC ON  CACHE BOOL "" FORCE)
    set(SDL_TEST   OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-2.30.9)
    FetchContent_MakeAvailable(SDL2)
    if(NOT TARGET SDL2::SDL2)
        add_library(SDL2::SDL2 ALIAS SDL2-static)  # normalise the target name
    endif()
endif()

# --- glm: math (header-only) ----------------------------------------------
find_package(glm QUIET)
if(NOT TARGET glm::glm)
    message(STATUS "glm not found on system; fetching from source")
    FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1)
    FetchContent_MakeAvailable(glm)
endif()

# ---------------------------------------------------------------------------
# Vendored libraries (always fetched + pinned by git tag)
# ---------------------------------------------------------------------------

# glad: generates a loader for exactly the target GL version. macOS caps desktop
# OpenGL at 4.1 (Apple deprecated GL), so match the runtime context in main.cpp.
if(APPLE)
    set(GAMEENGINE_GL_VERSION 4.1)
else()
    set(GAMEENGINE_GL_VERSION 4.6)
endif()
FetchContent_Declare(glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v2.0.8
    SOURCE_SUBDIR  cmake)
FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl STATIC REPRODUCIBLE API gl:core=${GAMEENGINE_GL_VERSION})

# Dear ImGui (docking branch) — no CMakeLists of its own, so build a target here
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking)
FetchContent_MakeAvailable(imgui)
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PUBLIC SDL2::SDL2)

# ---------------------------------------------------------------------------
# Engine executable
# ---------------------------------------------------------------------------
add_executable(engine src/main.cpp)
target_link_libraries(engine PRIVATE SDL2::SDL2 glm::glm glad_gl imgui)

# ---------------------------------------------------------------------------
# Sanitizers (Debug only, opt-out via -DGAMEENGINE_ENABLE_ASAN=OFF)
# ---------------------------------------------------------------------------
function(gameengine_apply_sanitizers target)
    if(GAMEENGINE_ENABLE_ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(MSVC)
            # MSVC ships AddressSanitizer but has no UBSan; the flag is enough
            # (the linker pulls in the ASan runtime automatically).
            target_compile_options(${target} PRIVATE /fsanitize=address)
        else()
            target_compile_options(${target} PRIVATE
                -fsanitize=address,undefined -fno-omit-frame-pointer)
            target_link_options(${target} PRIVATE
                -fsanitize=address,undefined)
        endif()
    endif()
endfunction()
gameengine_apply_sanitizers(engine)

# ---------------------------------------------------------------------------
# Unit tests (doctest, via pacman)
# ---------------------------------------------------------------------------
if(GAMEENGINE_BUILD_TESTS)
    find_package(doctest QUIET)
    if(NOT TARGET doctest::doctest)
        message(STATUS "doctest not found on system; fetching from source")
        FetchContent_Declare(doctest
            GIT_REPOSITORY https://github.com/doctest/doctest.git
            GIT_TAG        v2.4.12)
        FetchContent_MakeAvailable(doctest)
    endif()
    enable_testing()
    add_executable(engine_tests tests/test_main.cpp)
    target_link_libraries(engine_tests PRIVATE doctest::doctest glm::glm)
    gameengine_apply_sanitizers(engine_tests)
    add_test(NAME engine_tests COMMAND engine_tests)
endif()
