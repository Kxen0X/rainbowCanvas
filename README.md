# Rainbow Canvas

A procedural canvas of randomly colored chunks, written in C++ with SFML 3. Pan across the canvas and zoom in to explore the generated colors.

## Features

- Chunks generated on demand around the current camera view.
- A background worker thread for generating colored tiles.
- A distance-prioritized work queue and removal of chunks far from the camera.
- Random RGB colors assigned as chunks are created.
- Camera movement with the right mouse button.
- Mouse-wheel zoom through `sf::View`.
- A borderless window sized to the desktop.

This version is a canvas navigation experiment. It does not yet provide painting, image export, or saved canvases.

## Controls

| Input | Action |
| --- | --- |
| Hold **right mouse button** and drag | Pan the view |
| Scroll **up** | Zoom in |
| Scroll **down** | Zoom out |
| **Esc** | Close the application |

Tiles are generated as you explore. Unloaded tiles are generated again with new colors when revisited. Scroll down to see a larger area; the current code limits zooming out based on the view width.

## Build

Requirements: a C++17 compiler, CMake **3.28+**, Git, and Ninja. On Windows, use a terminal configured for the selected compiler.

The build downloads **SFML 3.0.2** through CMake `FetchContent` and links its Graphics and Audio modules. Internet access is needed for the first configuration.

```sh
git clone https://github.com/Kxen0X/rainbowCanvas.git
cd rainbowCanvas
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

For Linux system packages, see the dependency installation step in [.github/workflows/ci.yml](.github/workflows/ci.yml).

## Run

The final version no longer loads a background texture. The executable produced by the Ninja build is in **`build/bin`**.

Linux/macOS:

```sh
cd build/bin
./main
```

Windows PowerShell, from the repository root:

```powershell
Set-Location build/bin
.\main.exe
```

The inherited CMake script still copies the resource directory, but canvas rendering does not depend on that image.

## How it works

`checkArea()` identifies missing chunks around the view and sorts queued positions by distance from the camera. A worker running `createChunk()` generates colored rectangles. The render loop draws loaded chunks, and distant chunks are removed. Mouse input moves or scales an `sf::View`.

| File | Responsibility |
| --- | --- |
| [src/main.cpp](src/main.cpp) | Entry point |
| [src/project.cpp](src/project.cpp) | Chunk scheduling, worker loop, camera controls, and rendering |
| [headers/project.h](headers/project.h) | Chunk keys, hash function, thread, and shared state |
| [CMakeLists.txt](CMakeLists.txt) | Build configuration and SFML dependencies |

Change `chSize` in the constructor to experiment with tile dimensions. The loading and eviction radii are calculated in `checkArea()`.

## Current limitations

This is a threading experiment, not a production streaming engine. The idle worker currently busy-waits, and access to the chunk map in `checkArea()` is not consistently protected by its mutex. Generated colors are not persisted, and panning is not scaled to the current zoom. Synchronization, an idle condition variable, and persistent tile data are useful next steps.

The header uses `std::unordered_map` and `std::unordered_set` without explicitly including their headers. If a build reports those types as undeclared, add `<unordered_map>` and `<unordered_set>` to `headers/project.h`.

## License and credits

The build configuration comes from the [SFML CMake project template](https://github.com/SFML/cmake-sfml-project). See [LICENSE.md](LICENSE.md) for the repository's existing license text and attribution. Separate licensing information for the bundled background image is not included.
