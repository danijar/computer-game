Computer Game
=============

This is a modular game engine architecture for grounded cross platform graphics applications. It provides modules for distinctive features and managers for communicatation between them.

The application is thought out and developed by Danijar Hafner.

Screenshots
-----------

![Screenshot](screenshot/2014-02-08%20landscape.png?raw=true)

<img src="screenshot/2014-02-11%20normal%20maps%20and%20specular%20magic.png?raw=true" width="120">&nbsp;
<img src="screenshot/2014-03-02%20stairs%20day.png?raw=true" width="120">&nbsp;
<img src="screenshot/2014-03-02%20stairs%20night.png?raw=true" width="120">&nbsp;
<img src="screenshot/2014-01-24%20specular%20barrel.png?raw=true" width="120">

Features
--------

### State of Play

- Deferred Rendering
- Edge detect anti-aliasing
- Screen space ambient occlusion
- Scripting
- Physics and collision
- Dynamic character controller
- Procedural voxel based terrain
- Automated storing of entities to database
- Normal mapping and specular mapping

### Upcoming Features

- AI and character animations
- Modding support
- Editor capabilities

Take a look at the [Roadmap](ROADMAP.md) for an overview of all planned features.

### Feature Visions

- Shadow Volumes
- Terrain shape smoothing by a particular algorithm
- Realistic water by fluid simulation with [highly adaptive resolution](http://pub.ist.ac.at/group_wojtan/projects/2013_Ando_HALSoTM/index.html)
- Dynamic weather and daycycle system

Libraries
---------

- [Assimp](http://assimp.sourceforge.net/) Open Asset Import Library (3.0)
- [Bullet](http://bulletphysics.org/) Realtime Physics Simulation (2.82)
- [GLEW](http://glew.sourceforge.net/) OpenGL Extension Wrangler (1.10.0)
- [GLM](http://glm.g-truc.net/) OpenGL Mathematics (0.9.5.2)
- [libzip](http://www.nih.at/libzip/) Zip Archives (0.11.1)
- [SFML](http://www.sfml-dev.org/) Multimedia Library (2.1)
- [SQLite](http://www.sqlite.org/) SQL Database Engine (3.8.1)
- [V8](https://developers.google.com/v8/) JavaScript Engine (3.24.40)
- [zlib](http://www.zlib.net/) General Purpose Compression (1.2.8)

Instructions
------------

If you just want to play the game, the Windows executables [available for each release](https://github.com/danijar/computer-game/releases) are what you need. Just click the green button under a release to download the archive and run the `*.exe` file. This might require a newer Windows version.

There are no executables for Linux or Mac at the moment. However, the code is cross platform compatible and you will see at least Linux releases in future.

For development, there are Visual Studio 2013 project files included. I also provide prebuild libraries for the Visual C++ compiler. Read the [Binaries](bin/BINARIES.md) instructions for more details. Currently, there are no build scripts or prebuild libraries for other platforms. If you figured out how to build the code on another platform, please contribute to this project.

Keyboard controls
-----------------

### Settings and global actions

- `Esc` Exit the application.
- `F1` Toggle mouse capture for camera movement.
- `F2` Toggle wire frame mode.
- `F3` Toggle collision shapes debug draw.
- `F4` Toggle vertical sync.
- `F11` Toggle between fullscreen and windowed mode.
- `Tab` Toggle scripting console.

### Movement and orientation

- `W`, `A`, `S`, `D` or `Up`, `Down`, `Left`, `Right` Move camera.
- `Space` Jump.
- Hold `Left Shift` to move and jump even in the air and faster.

### Interaction with the world

- `Mouse Left` Mine selected block.
- `Mouse Right` Place a block.
- `Mouse Middle` Pick type of selected block.
- `1` to `9` Change type of block getting placed.

### Inserting test objects

- `Ctrl` + `C` Insert a capsule body.
- `Ctrl` + `V` Shoot a cube in view direction.
- `Ctrl` + `B` Place barrel on the floor.
- `Ctrl` + `N` Insert a big rock.
- `Ctrl` + `M` Set a shrine.
- `Ctrl` + `X` Insert a big cube with the material in global `texture` scripting variable.
- `H` + `O` + `U` + `S` + `E` Insert a procedurally generated house.

Savegames
---------

Savegames are stored in `/save/`. Modify the settings file located at `/script/settings/init.js` to specify which save game should be loaded at startup. The default name is `world`. If the name doesn't exist yet, a new world is created.

    Savegame: [ 'string', 'world' ],

There are also scripting functions available for creating new worlds and switching between them at run time.

Scripting
---------

The used scripting language is JavaScript.

As described above, the scripting console can be accessed by pressing the `Tab` key. However, it is only visible in debug mode which can be turned off at startup. In this case there is a scripting function to enter debug mode, which you'll have to type in blindly since the console isn't visible before.

To enable debug mode, blindly press `Tab`, type in `debug()` and hit `Enter`. You can now see debug information on the screen like the scripting console, frames per second, number of loaded meshes, coordinates of the camera, and so on.

For documentation of scripting callbacks, see the [List of Scripting Functions](https://github.com/danijar/computer-game/wiki/List-of-Scripting-Functions).

Questions
---------

If you have any questions, feel free to contact me. I'm sure you will find a way.
