# GameEngine — Learning Curriculum

A self-study plan to build a **2D and 3D** game engine **with an editor**, in
C++. **You write the code.** Each module gives you the concepts, what to build,
and a "done when" bar — not the implementation.

The path is: master 2D + the editor first (that teaches every fundamental), then
extend the *same core* to 3D. The editor, ECS, serialization, and gizmos all
carry over — 3D mostly adds a new rendering path plus a perspective camera.

### Timeline reality

2D + 3D done well is **~5–6 months** of nights/weekends, not 3. Phase 1 (2D +
editor) is a complete, portfolio-worthy project on its own; Phase 2 (3D) extends
it. If you must cap at ~3 months, do all of Phase 1 + a *minimal* Phase 2
(perspective camera, load and light one model in the editor) and skip the
standalone game.

## How to use this

- Work modules in order; each depends on the last.
- For every module: (1) learn the concepts, (2) build it yourself, (3) hit the
  "Done when" bar, (4) commit. Only then move on.
- **Write it before you look anything up beyond the listed concepts.** Struggling
  for 20 minutes teaches more than a working copy-paste.
- Keep a `DEVLOG.md` — one paragraph per session: what you built, what confused
  you, what you'd change. This becomes portfolio gold later.
- Use me as a tutor: ask for concept explanations, hints when stuck, and code
  review after you've written something. Don't ask me to write a module for you.

## Standing tools & habits

- **Sanitizers**: build Debug (ASan/UBSan on). When it crashes, read the report.
- **RenderDoc**: from Module 1 on, capture a frame whenever the screen looks
  wrong — inspect your draw calls, buffers, and textures.
- **GL debug callback**: already wired in `main.cpp`; watch stderr.
- **clang-format** before every commit; keep CI green. Commit small.

---

# PHASE 1 — 2D engine + editor

## Module 0 — Understand the bootstrap *(reading, ~1 session)*

You didn't write `src/main.cpp` — so first, understand every line of it.

- **Concepts**: what an OpenGL *context* is; the render loop; double buffering &
  `SwapWindow`; VAO vs VBO vs vertex attributes; the vertex→fragment pipeline;
  NDC coordinates; ImGui's immediate-mode model; the CMake build graph
  (find_package vs FetchContent, targets, linking).
- **Exercises**: comment each block; change the triangle's colors then shape;
  **turn the triangle into a quad** (two triangles or an index buffer), by hand.
- **Done when**: you can explain VAO vs VBO, why we double-buffer, and what
  `glDrawArrays` tells the GPU to do.

## Module 1 — Rendering abstractions *(~week 1–2)*

Wrap raw OpenGL in your own classes. Design them so a 3D renderer can reuse them.

- **Concepts**: RAII / rule of five / move semantics; who owns a GPU resource;
  pipeline stages; uniforms; texture sampling; sRGB & texture flipping.
- **Build**: `Shader` (load/compile/link, set uniforms), `VertexBuffer`,
  `IndexBuffer`, `VertexArray` with a `BufferLayout`, `Texture2D` (load PNG via
  **stb_image**, fetched by CMake).
- **Done when**: `main` draws a **textured quad** using *only* your classes — no
  raw `gl*` left in `main.cpp`. No leaks under ASan.
- **Pitfalls**: forgetting to bind; re-querying uniform locations each frame
  (cache them); upside-down textures.
- **Resources**: LearnOpenGL "Getting Started"; TheCherno OpenGL series.

## Module 2 — 2D camera & coordinate systems *(~week 2)*

- **Concepts**: model/world/view/clip/NDC/screen spaces; orthographic
  projection; the MVP matrix; column-major GLM.
- **Build**: an `OrthographicCamera` producing a `viewProjection` matrix; pan +
  zoom. Feed the matrix to your shader.
- **Done when**: WASD pans and scroll zooms over quads at different world
  positions, and they stay put in the world.
- **Pitfalls**: matrix multiply order; confusing view (inverse camera) with model.

## Module 3 — 2D batch renderer *(~week 3–4)* — *signature subsystem*

The "I understand graphics performance" module. Push it past a tutorial and it
becomes a portfolio highlight.

- **Concepts**: draw-call overhead; batching; one large dynamic VBO; per-vertex
  texture indices; texture slots & the sampler limit; when to *flush*.
- **Build**: `Renderer2D` with `BeginScene(camera)` / `DrawQuad(...)` /
  `EndScene()`. Accumulate vertices CPU-side, upload once, draw in one call;
  flush when the batch or texture slots fill.
- **Done when**: **thousands** of sprites at 60 fps, with an ImGui **draw-call
  counter** that stays low.
- **Pitfalls**: reallocating the VBO every frame (`glBufferSubData`); exceeding
  texture units; not resetting stats each frame.
- **Stretch**: rotated quads; tinting; a particle system.

## Module 4 — ECS & the Scene *(~week 5)*

- **Concepts**: data-oriented design; ECS vs deep inheritance; EnTT registry,
  entities, components, views; the game's *data model*.
- **Build**: add **EnTT** (FetchContent). A `Scene` wrapping `entt::registry`; an
  `Entity` handle; components `TagComponent`, `TransformComponent`,
  `SpriteRendererComponent`. A render pass iterating a Transform+Sprite view.
- **Done when**: a code-built scene renders through your batch renderer via an
  EnTT view.
- **Pitfalls**: storing raw pointers to components (they move); fat components.
- **Resources**: EnTT wiki "Crash Course: ECS".

## Module 5 — Editor shell: framebuffer + docking *(~week 6)*

The moment it becomes "an engine with an editor."

- **Concepts**: off-screen rendering with an FBO; render-to-texture; ImGui
  docking / dockspace; drawing a GL texture in ImGui.
- **Build**: a `Framebuffer` (color + depth, resizable). Render the scene into it,
  then show its texture via `ImGui::Image` in a **"Viewport"** panel. Add a
  dockspace.
- **Done when**: the game renders in a dockable, resizable viewport panel;
  resizing resizes the FBO without stretching.
- **Pitfalls**: not resizing the FBO to the panel (blurry); flipped UVs; missing
  depth attachment (bites you in 3D).

## Module 6 — Hierarchy & inspector *(~week 7)*

- **Concepts**: editor-state vs runtime-state; immediate-mode UI patterns;
  selection; editing live data safely.
- **Build**: `SceneHierarchyPanel` (list tags, select, create/delete);
  `PropertiesPanel` editing the selected entity's components with ImGui widgets.
- **Done when**: create, select, rename, and edit entities entirely with the
  mouse, live in the viewport.
- **Note**: hand-write the inspector per component. A reflection-driven
  auto-inspector is a great *stretch*, not a blocker.

## Module 7 — Gizmos *(~week 7)*

- **Concepts**: manipulating transforms from screen space; ImGuizmo (**already
  3D** — this investment pays off in Phase 2).
- **Build**: integrate **ImGuizmo** (FetchContent) into the viewport; translate /
  rotate / scale the selection; hotkeys to switch mode.
- **Done when**: drag a gizmo to transform the selected object; the Properties
  panel updates in sync.

## Module 8 — Serialization *(~week 8)*

- **Concepts**: (de)serialization; a human-readable scene format; stable IDs.
- **Build**: a `SceneSerializer` to/from **YAML** (yaml-cpp, installed). Save /
  Open menu items.
- **Done when**: build a scene, save, quit, relaunch, open — exactly as left.
- **Pitfalls**: no stable ID component (relationships break); silent failure on
  bad files (validate + report).

**End of Phase 1 = a complete 2D engine + editor. Tag `v1.0-2d`.**

---

# PHASE 2 — 3D engine (built on the same core)

The editor, ECS, framebuffer, gizmos, and serialization all carry over. You're
mainly adding a 3D rendering path and a perspective camera.

## Module 9 — 3D foundations *(~week 9)*

- **Concepts**: perspective projection & FOV; the **depth buffer** & depth
  testing; back-face culling; 3D coordinate handedness; why the FBO needed a
  depth attachment.
- **Build**: a `PerspectiveCamera`; enable `GL_DEPTH_TEST`; render a **spinning
  lit-later cube** from mesh data. Add an **editor camera** (orbit or fly with
  mouse-look) so you can navigate the 3D viewport.
- **Done when**: a cube renders correctly with depth in the editor viewport, and
  you can fly/orbit the camera around it.
- **Pitfalls**: forgetting to clear the depth buffer; no depth attachment on the
  FBO; near/far plane set wrong (z-fighting or clipping).

## Module 10 — Meshes & model loading *(~week 10)*

- **Concepts**: mesh data (positions, normals, UVs, indices); vertex normals;
  what a material is; import vs engine-native formats.
- **Build**: a `Mesh` / `Model` class; load a real model. Use **tinyobjloader**
  (OBJ, simplest) or **tinygltf** (glTF, modern) via FetchContent — or `assimp`
  from pacman if you want many formats.
- **Done when**: you load and render a textured 3D model (e.g. a glTF/OBJ) in the
  viewport.
- **Pitfalls**: winding order / inverted normals; not uploading an index buffer;
  huge models tanking load time (keep test assets small).

## Module 11 — Lighting & materials *(~week 11)*

- **Concepts**: the lighting equation; **Blinn-Phong** (ambient + diffuse +
  specular); directional vs point lights; the normal matrix; gamma.
- **Build**: a lighting shader with a directional light + at least one point
  light; a simple `Material` (albedo/tint, shininess). Light components in the ECS.
- **Done when**: your loaded model is lit and shaded; moving a light visibly
  changes it.
- **Pitfalls**: transforming normals with the model matrix instead of the normal
  matrix; specular in the wrong space.
- **Stretch**: swap Blinn-Phong for a basic **PBR** (metallic/roughness) — a
  strong portfolio flex.

## Module 12 — 3D in the editor & ECS *(~week 12)*

- **Concepts**: unifying 2D + 3D behind one renderer interface; a scene grid;
  3D gizmos (ImGuizmo already does this).
- **Build**: a `MeshComponent` + `MaterialComponent`; extend the render system to
  draw 3D entities; a ground grid; ImGuizmo operating in the perspective view.
- **Done when**: you assemble a small 3D scene in the editor — place models,
  move them with gizmos, add lights — and save/load it through your existing
  serializer.

**End of Phase 2 = a 2D + 3D engine + editor. Tag `v2.0`.**

---

# PHASE 3 — ship it

## Module 13 — Demo *(~week 13)* — *headline artifact*

- **Concepts**: native "script" components (`OnCreate` / `OnUpdate`); the
  editor's **Play / Stop** cycle; deep-copying the scene so Play doesn't mutate
  your saved edit-state.
- **Build**: a small, **complete** experience built in your editor — a 2D
  mini-game or a navigable/interactive 3D scene. Add Play/Stop.
- **Done when**: someone opens your editor, hits Play, and interacts with
  something real you assembled with your own tools.

## Module 14 — Polish & portfolio *(~week 14 + buffer)*

- README with **GIFs + a 60–90s video** of the editor (2D and 3D).
- A short **devblog** on one hard problem (batch renderer, edit-vs-runtime state,
  or the lighting pipeline) — signals seniority.
- CI green on GCC **and** Clang; run **clang-tidy** and fix warnings.
- Tag a release; write "what I learned / what's next".

---

## Scope-cut order (if you're running behind)

Drop in this order — none breaks the story: standalone demo game → PBR (keep
Blinn-Phong) → undo/redo → reflection-driven inspector (hand-write) → audio.
**Protect at all costs**: the editor viewport, the inspector, working 2D *and* a
minimal-but-real 3D scene.

## Core resources

- **LearnOpenGL** (learnopengl.com) — 2D *and* 3D (lighting, model loading, PBR).
- **TheCherno** "Game Engine" (Hazel) + "OpenGL" series — builds almost exactly
  the 2D half. Use for concepts; write your own; push one subsystem past his.
- **EnTT wiki** — ECS. **Dear ImGui** `imgui_demo.cpp` — widget reference.
- **Game Engine Architecture** (Jason Gregory) — the big-picture book.

## Progress

- [x] Module 0 — Understand the bootstrap
- [ ] Module 1 — Rendering abstractions
- [ ] Module 2 — 2D camera & coordinate systems
- [ ] Module 3 — 2D batch renderer
- [ ] Module 4 — ECS & Scene
- [ ] Module 5 — Editor shell (framebuffer + docking)
- [ ] Module 6 — Hierarchy & inspector
- [ ] Module 7 — Gizmos
- [ ] Module 8 — Serialization  *(end Phase 1)*
- [ ] Module 9 — 3D foundations
- [ ] Module 10 — Meshes & model loading
- [ ] Module 11 — Lighting & materials
- [ ] Module 12 — 3D in the editor & ECS  *(end Phase 2)*
- [ ] Module 13 — Demo
- [ ] Module 14 — Polish & portfolio
