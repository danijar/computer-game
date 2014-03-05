Graphics Application
====================

This is a modular game engine architecture for grounded cross platform graphics applications. It provides managers for messaging, entities and automatedly storing them in databases. Additionally, it provides functionality for unified logging, scripting, file access and asset management. Currently implemented modules cover OpenGL rendering, physics simulation, text drawing, skydome, character control, voxel based terrain and more.

The application is thought out and written by Danijar Hafner.

Screenshots
-----------

![Screenshot](screenshot.png?raw=true)

<img src="screenshot/2014.02.11%20normal%20maps%20and%20specular%20magic.png?raw=true" width="150">&nbsp;
<img src="screenshot/2014.03.02%20stairs%20day.png?raw=true" width="150">&nbsp;
<img src="screenshot/2014.03.02%20stairs%20night.png?raw=true" width="150">&nbsp;
<img src="screenshot/2014.01.24%20specular%20barrel.png?raw=true" width="150">


Features
--------

State of Play

- Deferred Rendering
- Edge detect anti-aliasing
- Screen space ambient occlusion
- Scripting
- Physics and collision
- Dynamic character controller
- Procedural voxel based terrain
- Automated storing of entities to database
- Normal mapping

Upcoming Features

- AI and character animations
- Modding support
- Editor capabilities

Take a look at the [Roadmap](https://raw.github.com/ComputerGame/GraphicsApplication/master/ROADMAP.md) for an overview of all planned features.

Feature Visions

- Shadow Volumes
- Terrain shape smoothing by a particular algorithm
- Realistic water by fluid simulation with [highly adaptive resolution](http://pub.ist.ac.at/group_wojtan/projects/2013_Ando_HALSoTM/index.html)
- Dynamic weather and daycycle system

Libraries
---------

- [Assimp](http://assimp.sourceforge.net/) Open Asset Import Library (3.0)
- [Bullet](http://bulletphysics.org/) Realtime Physics Simulation (2.81)
- [GLEW](http://glew.sourceforge.net/) OpenGL Extension Wrangler (1.9.0)
- [GLM](http://glm.g-truc.net/) OpenGL Mathematics (0.9.4.1)
- [libzip](http://www.nih.at/libzip/) Zip Archives (0.11.1)
- [SFML](http://www.sfml-dev.org/) Multimedia Library (2.0 RC)
- [SQLite](http://www.sqlite.org/) SQL Database Engine (3.8.1)
- [V8](https://developers.google.com/v8/) JavaScript Engine (30.03.2013)
- [zlib](http://www.zlib.net/) General Purpose Compression (1.2.8)

Instructions
------------

There is a precompiled executable for Windows. To use it, just download the repository and run `/main.exe` from the repository root. You may need a recent version of Windows.

Currently, there is no compiler setup for Linux. However, the source code is cross platform and you will see a Linux release in future. Until then you may build this application on your own, but keep in mind that you need to build all dependencies listed above first.

The code should be compatible to Mac, but there are no plans for support.

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

Inserting test objects

- `C` Insert a capsule body.
- `V` Shoot a cube in view direction.
- `B` Place barrel on the floor.
- `N` Insert a big rock.
- `M` Set a shrine.
- `X` Insert a big cube with the material in global `texture` scripting variable.

### Saves

The game sames are stored in a `/save/` folder. Modify the settings file located at `/module/settings/init.js` to specify which save game should be loaded at startup. The default name is `world`. If the name doesn't exist yet, a new world is created.

    Savegame: [ 'string', 'world' ],

There are also scripting functions available for creating new worlds and switching between them at run time.

### Scripting

The scripting language used is JavaScript.

As described above, the scripting console can be accessed by pressing the `Tab` key. Though it is only visible in debug mode. There is a scripting function to enter debug mode, which you'll have to type in blindly since the console isn't visible yet.

To enable debug mode, blindly press `Tab`, type in `debug()` and hit `Enter`. You can now see debug information on the screen like the scripting console, frames per second, number of loaded meshes, coordinates of the camera, and so on.

For a full list of scripting functions, see the [List of Scripting Functions](https://github.com/ComputerGame/GraphicsApplication/wiki/List-of-Scripting-Functions).

### Questions

If you have any questions, feel free to contact me. I'm sure you will find a way.
