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
# System libraries (installed via pacman)
# ---------------------------------------------------------------------------
find_package(SDL2 REQUIRED)   # window + input   (sdl2-compat)
find_package(glm REQUIRED)    # math

# ---------------------------------------------------------------------------
# Vendored libraries (fetched + pinned by git tag)
# ---------------------------------------------------------------------------

# glad: generates a loader for exactly OpenGL 4.6 core
FetchContent_Declare(glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v2.0.8
    SOURCE_SUBDIR  cmake)
FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl STATIC REPRODUCIBLE API gl:core=4.6)

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
        target_compile_options(${target} PRIVATE
            -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE
            -fsanitize=address,undefined)
    endif()
endfunction()
gameengine_apply_sanitizers(engine)

# ---------------------------------------------------------------------------
# Unit tests (doctest, via pacman)
# ---------------------------------------------------------------------------
if(GAMEENGINE_BUILD_TESTS)
    find_package(doctest REQUIRED)
    enable_testing()
    add_executable(engine_tests tests/test_main.cpp)
    target_link_libraries(engine_tests PRIVATE doctest::doctest glm::glm)
    gameengine_apply_sanitizers(engine_tests)
    add_test(NAME engine_tests COMMAND engine_tests)
endif()
