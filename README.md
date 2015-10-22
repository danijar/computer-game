Computer Game
=============

This is a modular voxel game engine. It features an ECS architecutre,
message-passing system and advanced rendering techniques. I started rewriting
the engine from scratch a while ago with multi-threading built in from the
beginning and an automated cross-platform build system. Here you can find
[the new repository][1] which is work in progress.

[1]: https://github.com/danijar/modurale

![Screenshot](screenshot/2014-02-08%20landscape.png?raw=true)

<img src="screenshot/2014-02-11%20normal%20maps%20and%20specular%20magic.png?raw=true" width="120">&nbsp;
<img src="screenshot/2014-03-02%20stairs%20day.png?raw=true" width="120">&nbsp;
<img src="screenshot/2014-03-02%20stairs%20night.png?raw=true" width="120">&nbsp;
<img src="screenshot/2014-01-24%20specular%20barrel.png?raw=true" width="120">

Features
--------

- Deferred Rendering
- Edge detect anti-aliasing
- Screen space ambient occlusion
- Scripting
- Physics and collision
- Dynamic character controller
- Procedural voxel based terrain
- Automated storing of entities to database
- Normal mapping and specular mapping

Dependencies
------------

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

If you just want to play the game on Windows, use the [available binaries for
each release][2] are what you need.  Just click the green button under a
release to download the archive and run the `*.exe` file. This might require a
newer Windows version.

For development, there are Visual Studio 2013 project files included. I also
provide prebuild libraries for the Visual C++ compiler. You need to build the
dependencies and put thier static LIB files into the `bin` directory first.

[2]: https://github.com/danijar/computer-game/releases

Savegames
---------

Savegames are stored in `save/`. Modify the settings file located at
`script/settings/init.js` to specify which save game should be loaded at
startup. The default name is `world`. If the name doesn't exist yet, a new
world is created.

    Savegame: [ 'string', 'world' ],

There are also scripting functions available for creating new worlds and
switching between them at run time.

Scripting
---------

The scripting language is JavaScript. You can access the in-game scripting
console by pressing `Tab`. It's only visible in debug mode, though. To enable
debug mode, blindly press `Tab`, type in `debug()` and hit `Enter`. You can now
see debug information on the screen like the scripting console, frames per
second, number of loaded meshes, coordinates of the camera, and so on.

Documentation can be found in the [List of Scripting Functions][3].

[3]: https://github.com/danijar/computer-game/wiki/List-of-Scripting-Functions

Controls
--------

### Settings and global actions

| Key   | Action                                       |
| :---: | -------------------------------------------- |
| `Esc` | Exit the application.                        |
| `F1`  | Toggle mouse capture for camera movement.    |
| `F2`  | Toggle wire frame mode.                      |
| `F3`  | Toggle collision shapes debug draw.          |
| `F4`  | Toggle vertical sync.                        |
| `F11` | Toggle between fullscreen and windowed mode. |
| `Tab` | Toggle scripting console.                    |

### Interaction with the world

| Key            | Action                               |
| :------------: | ------------------------------------ |
| `Mouse Left`   | Mine selected block.                 |
| `Mouse Right`  | Place a block.                       |
| `Mouse Middle` | Pick type of selected block.         |
| `1` to `9`     | Change type of block getting placed. |

### Inserting test objects

| Key      | Action                                                           |
| :------: | ---------------------------------------------------------------- |
| `Ctrl+C` | Insert a capsule body.                                           |
| `Ctrl+V` | Shoot a cube in view direction.                                  |
| `Ctrl+B` | Place barrel on the floor.                                       |
| `Ctrl+N` | Insert a big rock.                                               |
| `Ctrl+M` | Set a shrine.                                                    |
| `Ctrl+X` | Insert a big cube with the material from the `texture` variable. |
| `HOUSE`  | Insert a procedurally generated house.                           |

Known Bugs
----------

- Switch to fullscreen doen't work
- Testing on intel tablet, the screen pass doesn't work after resize
- Material live reload is not reliable
- Two unneeded framebuffer creations fail at startup
- Stencil doesn't work in all cases
- Out of range chunks aren't freed completely
- One can sometimes jump when in air
