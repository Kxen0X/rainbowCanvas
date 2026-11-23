# Rainbow Canvas

An interactive grid of randomly colored tiles, written in C++ with SFML 3. Pan across the canvas and zoom in to explore the generated colors.

## Features

- A **100 × 100 grid** containing 10,000 colored rectangles.
- Random RGB colors generated when the application starts.
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

Colors are regenerated on each launch. The initial camera view is deliberately small relative to the full grid; scroll down to see more tiles.

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

The program loads `../resources/images.jpg` during initialization, even though drawing that background is currently disabled. Keep the resource file and launch with **`build/bin` as the working directory**.

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

CMake copies the assets to `build/resources`. If you launch through an IDE, set its working directory to `build/bin` as well.

## How it works

`Project::initChunks()` creates the tile grid and assigns each tile a random color. Mouse input moves or scales an `sf::View`; the render loop draws the rectangles through that view.

| File | Responsibility |
| --- | --- |
| [src/main.cpp](src/main.cpp) | Entry point |
| [src/project.cpp](src/project.cpp) | Grid generation, camera controls, and rendering |
| [headers/project.h](headers/project.h) | Canvas state and `Chunk` structure |
| [CMakeLists.txt](CMakeLists.txt) | Build configuration and SFML dependencies |

Change `width`, `height`, and the tile-size argument to `initChunks()` in the constructor to experiment with different canvases.

## Current limitations

All 10,000 tiles are drawn every frame; viewport culling is not implemented. Zoom is unrestricted, and dragging is not adjusted for the current zoom level. These are useful areas for further work alongside drawing tools and canvas persistence.

## License and credits

The build configuration comes from the [SFML CMake project template](https://github.com/SFML/cmake-sfml-project). See [LICENSE.md](LICENSE.md) for the repository's existing license text and attribution. Separate licensing information for the bundled background image is not included.
