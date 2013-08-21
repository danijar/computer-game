Graphics Application: Terrain
=============================

This is a branch of a basic [Graphics Application](https://github.com/ComputerGame/GraphicsApplication) extending the functionality by voxel based terrain capabilities.

Features
--------

- Random terrain generation using multiple layers of [Simplex noise](http://en.wikipedia.org/wiki/Simplex_noise)
- Culled mesh generation from voxel data
- Different block looks using a texture atlas
- Terrain modification at runtime
- Store and load world and player from disk

Screenshot
----------

![Screenshot](https://raw.github.com/ComputerGame/GraphicsApplication/terrain/screenshot.png)

Instructions
------------

There is a precompiled executable for Windows. To use it, download the content of the `Application` folder and run the executable. You may need a recent Windows version to run this.

### Key controls

Settings and global actions

- `Esc` Exit the application.
- `F1` Toggle mouse capture for camera movement.
- `F2` Toggle wire frame mode.
- `F3` Toggle collision shapes debug draw.
- `F4` Toggle vertical sync.
- `F11` Toggle between fullscreen and windowed mode.
- `Tab` Toggle scripting console.

Movement and orientation

- `W`, `A`, `S`, `D` or `Up`, `Down`, `Left`, `Right` Move camera.
- `Space` Jump.
- Hold `Left Shift` to move and jump even in the air and faster.

Interaction with the world

- `Mouse Left` Mine selected block.
- `Mouse Right` Place a block.
- `Mouse Middle` Pick type of selected block.
- `1` to `9` Change type of block getting placed.
- `F` Shoot a cube in view direction.

Upcoming Features
-----------------

- Generation of more simple meshes with adjacent face merging
- Terrain smoothing by a particular algorithm
- Realistig water by fluid simulation with (highly adaptive resolution)[http://pub.ist.ac.at/group_wojtan/projects/2013_Ando_HALSoTM/index.html]
