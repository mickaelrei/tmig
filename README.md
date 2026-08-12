# tmig

C++ / OpenGL 4.4 rendering library. The goal is to be consumed by other projects (git submodule, `add_subdirectory`, etc.) as a thin API over OpenGL resources: windows, shaders, meshes, textures, framebuffers, instancing, lights and post-processing.

It is not a full game engine. You keep ownership of your scene; tmig wraps GPU objects and the frame loop plumbing.

## Features

- `render::Window`, `ShaderProgram`, `Texture2D`, `Framebuffer`
- `Mesh` / `InstancedMesh` with explicit vertex layouts and GPU buffers
- `UniformBuffer` (std140) and `core::LightManager` (directional / point / spot)
- Post-processing effects (`BloomEffect`, `BlurEffect`)
- Input, camera controllers and ImGui (`render::ui`)
- Built-in mesh generators (box, sphere, torus, …)

## Requirements

- Linux (tested on Debian/Ubuntu and Fedora)
- C++ compiler with C++17 (g++ / clang++)
- CMake 3.12+
- OpenGL 4.4 driver
- GLFW 3
- GLM
- Git (ImGui is always built from `external/imgui`; Assimp is used from the system if present, otherwise from `external/assimp`)

## Install dependencies

### Debian / Ubuntu

```bash
sudo apt update
sudo apt install --no-install-recommends \
    build-essential cmake git \
    libgl1-mesa-dev libglfw3-dev libglm-dev libassimp-dev
```

`libassimp-dev` is optional. If you skip it, CMake pulls and builds the Assimp submodule.

### Fedora

```bash
sudo dnf install \
    gcc-c++ cmake git \
    mesa-libGL-devel glfw-devel glm-devel assimp-devel
```

`assimp-devel` is optional for the same reason as above.

## Build

Clone with submodules (ImGui is required; Assimp only if you do not have a system package):

```bash
git clone --recursive https://github.com/mickaelrei/tmig.git
cd tmig
```

If you already cloned without `--recursive`:

```bash
git submodule update --init --recursive
```

Configure and build from a separate directory:

```bash
cmake -B build -DTMIG_BUILD_TESTS=ON
cmake --build build -j
```

Useful options:

| Option | Default | Description |
| --- | --- | --- |
| `TMIG_BUILD_TESTS` | `OFF` | Build the demo scenes under `tests/` |
| `ENGINE_CONFIG` | `Release` | `Debug`, `RelWithDebInfo` or `Release` (Debug enables ASan/UBSan) |
| `DEBUG` | `OFF` | Extra GL/engine logging |

The shared library is written to `lib/`. Test binaries go to `tests/bin/`.

## Demo scenes

Built only with `-DTMIG_BUILD_TESTS=ON`. Right-click to look, WASD to move, Esc to quit. Each window has an ImGui panel.

```bash
./tests/bin/instanced     # instanced vs non-instanced + high/low-poly LOD
./tests/bin/framebuffer   # off-screen FBO + post-process kernels
./tests/bin/bloom         # HDR neon plaza + bloom (split view)
./tests/bin/lights        # closed room, orbiting point lights, flashlight
```

`instanced` is the right place to compare draw-call cost: toggle instancing and LOD in the UI and watch the FPS in the title bar.

## Use as a git submodule

```bash
git submodule add https://github.com/mickaelrei/tmig.git external/tmig
git submodule update --init --recursive
```

In your project's `CMakeLists.txt`:

```cmake
add_subdirectory(external/tmig)
target_link_libraries(your_app PRIVATE tmig)
```

Leave `TMIG_BUILD_TESTS` off in the parent project. Public headers live in `include/`:

```cpp
#include "tmig/render/render.hpp"
#include "tmig/render/window.hpp"
#include "tmig/render/shader.hpp"
#include "tmig/render/mesh.hpp"
```

`util::getResourcePath(...)` resolves files under tmig's `resources/` (engine shaders, default textures). Your own assets should be loaded with your own paths.

## Minimal loop

```cpp
tmig::render::init();
tmig::render::setClearColor({0.05f, 0.05f, 0.08f, 1.0f});

while (!tmig::render::window::shouldClose()) {
    tmig::core::input::update();
    tmig::render::clearBuffers();
    // draw with Mesh / InstancedMesh after binding a ShaderProgram
    tmig::render::window::swapBuffers();
}
```

See `tests/` for complete scenes (UBOs, framebuffers, lights, bloom).
