Graphics Application
====================

Modular architecture for grounded cross platform graphics applications. Provides global managers for messaging, entities and automatedly storing them in databases. Comes with helpers for logging, scripting, file access and asset management. Implemented modules cover OpenGL graphics, physics simulation, text drawing, skydome, character control, voxel based terrain and more.

Screenshot
----------

![Screenshot](https://raw.github.com/ComputerGame/GraphicsApplication/master/screenshot.png)

Features
--------

### State of Play

- First person camera
- Deferred Rendering
- Basic text drawing
- Edge detect anti-aliasing
- Screen space ambient occlusion
- Deferred Lighting
- Scripting
- Physics and collision
- Dynamic character controller
- Random generated voxel based terrain

### Upcoming Features

- Normal mapping
- AI and character animations
- Modding support
- Editor capabilities

Take a look at the [Roadmap](https://raw.github.com/ComputerGame/GraphicsApplication/master/ROADMAP.md) for an overview of all planned features.

### Feature Visions

- Shadow Volumes
- Terrain shape smoothing by a particular algorithm
- Realistic water by fluid simulation with [highly adaptive resolution](http://pub.ist.ac.at/group_wojtan/projects/2013_Ando_HALSoTM/index.html)
- Dynamic weather and daycycle system

Libraries
---------

- [GLEW](http://glew.sourceforge.net/) OpenGL Extension Wrangler (1.9.0)
- [GLM](http://glm.g-truc.net/) OpenGL Mathematics (0.9.4.1)
- [SFML](http://www.sfml-dev.org/) Multimedia Library (2.0 RC)
- [Assimp](http://assimp.sourceforge.net/) Open Asset Import Library (3.0)
- [V8](https://developers.google.com/v8/) JavaScript Engine (30.03.2013)
- [Bullet](http://bulletphysics.org/) Realtime Physics Simulation (2.81)
- [SQLite](http://www.sqlite.org/) SQL Database Engine (3.8.1)
- [zlib](http://www.zlib.net/) General Purpose Compression (1.2.8)
- [libzip](http://www.nih.at/libzip/) Zip Archives (0.11.1)

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

### Scripting

The scripting language used is JavaScript.

As described above, the scripting console can be accessed by pressing the `Tab` key. Though it is only visible in debug mode. There is a scripting function to enter debug mode, which you'll have to type in blindly since the console isn't visible yet.

To enable debug mode, blindly press `Tab`, type in `debug()` and hit `Enter`. You can now see debug information on the screen like the scripting console, frames per second, number of loaded meshes, coordinates of the camera, and so on.

For a full list of scripting functions, see the [List of Scripting Functions](https://github.com/ComputerGame/GraphicsApplication/wiki/List-of-Scripting-Functions).
