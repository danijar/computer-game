Graphics Application
====================

This is an OpenGL graphics application based on a component and data oriented [architecture](https://github.com/ComputerGame/GraphicsArchitecture) with event manager.

Features
--------

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

Screenshot
----------

![Screenshot](https://raw.github.com/ComputerGame/GraphicsApplication/master/screenshot.png)

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

- `Mouse Right` Shoot a cube in view direction.
- `F` Let a cube fall down in the center of the world.
- Hold `Left Shift` to insert more and fast cubiods.

### Scripting

The scripting language used is JavaScript.

As described above, the scripting console can be accessed by pressing the `Tab` key. Though it is only visible in debug mode. There is a scripting function to enter debug mode, which you'll have to type in blindly since the console isn't visible yet.

To enable debug mode, blindly press `Tab`, type in `debug()` and hit `Enter`. You can now see debug information on the screen like the scripting console, frames per second, number of loaded meshes, coordinates of the camera, and so on.

For a full list of scripting functions, see the [List of Scripting Functions](https://github.com/ComputerGame/GraphicsApplication/wiki/List-of-Scripting-Functions).

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
- Character animations
- Artificial intelligence
- Combat
- Networking, for multiplayer
