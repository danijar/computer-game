Graphics Application: Terrain
=============================

<<<<<<< HEAD
This is a branch of a basic [Graphics Application](https://github.com/ComputerGame/GraphicsApplication) extending the functionality by voxel based terrain capabilities.
=======
This is an OpenGL graphics application based on a component and data oriented [architecture](https://github.com/ComputerGame/GraphicsArchitecture) with event manager.
>>>>>>> origin/master

Features
--------

<<<<<<< HEAD
- Random terrain generation using [Simplex noise](http://en.wikipedia.org/wiki/Simplex_noise)
- Culled mesh generation from voxel data
- Different block looks using a texture atlas
- Fog fading out far landscapes
=======
- First person camera
- Deferred Rendering
- Basic text drawing
- Edge detect anti-aliasing
- Screen space ambient occlusion
- Deferred Lighting
- Scripting
- Physics and collision
- Dynamic character controller

Libraries
---------

- [SFML](http://www.sfml-dev.org/) Multimedia Library (2.0 RC)
- [GLEW](http://glew.sourceforge.net/) OpenGL Extension Wrangler Library (1.9.0)
- [GLM](http://glm.g-truc.net/) OpenGL Mathematics (0.9.4.1)
- [Assimp](http://assimp.sourceforge.net/) Open Asset Import Library (3.0)
- [V8](https://developers.google.com/v8/) JavaScript Engine (30.03.2013)
- [Bullet](http://bulletphysics.org/) Physics Library (2.81)
>>>>>>> origin/master

Screenshot
----------

![Screenshot](https://raw.github.com/ComputerGame/GraphicsApplication/terrain/screenshot.png)

Instructions
------------

There is a precompiled executable for Windows. To use it, download the content of the `Application` folder and run the executable. You may need a recent Windows version to run this.

<<<<<<< HEAD
Also see the [key controls](https://github.com/ComputerGame/GraphicsApplication/#key-controls) of the main application.
=======
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

- `Mouse Left` Shoot a cube in view direction.
- `F` Let a cube fall down in the center of the world.
- Hold `Left Shift` to insert more and fast cubiods.

Branches
--------

The [Terrain](https://github.com/ComputerGame/GraphicsApplication/tree/terrain) branch implements a voxel based world. It involves chunk management, a terrain generator and meshing techniques.
>>>>>>> origin/master

Upcoming Features
-----------------

<<<<<<< HEAD
- Generation of more simple meshes with adjacent face merging
- Block modification at runtime
- Terrain smoothing by a particular algorithm
=======
There is a lot to do and the core development hasn't finished yet.

- Normal Mapping
- Shadow Volumes

Desired features that might be implemented in future.

- Global illumination
- Character animations
- Artificial intelligence
- Combat
- Networking, for multiplayer
>>>>>>> origin/master
