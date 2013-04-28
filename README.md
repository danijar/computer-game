Graphics Application
====================

This is an OpenGL graphics application using a component and data based [Graphics Architecture](https://github.com/ComputerGame/GraphicsArchitecture) with event manager.

Features
--------

- First person camera
- Deferred Rendering
- Basic text drawing
- Edge detect anti-aliasing
- Screen space ambient occlusion
- Deferred Lighting
- Scripting
- Physics and Collision

Libraries
---------

- [SFML](http://www.sfml-dev.org/) Multimedia Library (2.0 RC)
- [GLEW](http://glew.sourceforge.net/) OpenGL Extension Wrangler Library (1.9.0)
- [GLM](http://glm.g-truc.net/) OpenGL Mathematics (0.9.4.1)
- [Assimp](http://assimp.sourceforge.net/) Open Asset Import Library (3.0)
- [V8](https://developers.google.com/v8/) JavaScript Engine (30.03.2013)
- [Bullet](http://bulletphysics.org/) Physics Library (2.81)

Screenshot
----------

![Screenshot](https://raw.github.com/ComputerGame/GraphicsApplication/master/screenshot.png)

Instructions
------------

There is a precompiled executable for Windows. To use it, download the content of the `Application` folder and run the executable.

### Key controls

- `Esc` Exit the application.
- `F1` Toggle mouse capture for camera movement.
- `F2` Toggle vertical sync.
- `F3` Toggle wire frame mode.
- `F4` Toggle antialiasing.
- `F11` Toggle between fullscreen and windowed mode.
- `Tab` Let a cube fall down in the center of the world.
- `Left Shift` + `Tab` Let twenty cubes fall down in the center of the world with a strong impulse.
- `Mouse Left` Shoot a cube in view direction.
- `Up`, `Down`, `Left`, `Right` or `W`, `A`, `S`, `D` Move camera.
- `PageUp`, `PageDown` or `Q`, `E` Raise and lower the camera.
- Hold `Left Shift` to move faster.

Branches
--------

The [Terrain](https://github.com/ComputerGame/GraphicsApplication/tree/terrain) branch implements a voxel based world. It involves chunk management, a terrain generator and meshing techniques.

Upcoming Features
-----------------

There is a lot to do and the core development hasn't finished yet.

- Normal Mapping
- Shadow Volumes

Desired features that might be implemented in future.

- Global illumination
- Characters
- Animations
- Pathfinding
- Artificial intelligence
- Combat
- Networking, for multiplayer
