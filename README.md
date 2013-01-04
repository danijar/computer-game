Graphics Application
====================

This is an OpenGL graphics application using a component and data based [Graphics Architecture](https://github.com/ComputerGame/GraphicsArchitecture) with event manager.

Features
--------

- Camera
- Movement
- Textures
- Basic Voxel Lighting

Libraries
---------

- [SFML](http://www.sfml-dev.org/) Multimedia Library (2.0 RC)
- [GLEW](http://glew.sourceforge.net/) OpenGL Extension Wrangler Library (1.9.0)
- [GLM](http://glm.g-truc.net/) OpenGL Mathematics (0.9.4.1)

Screenshot
----------

![Screenshot](https://raw.github.com/ComputerGame/GraphicsApplication/master/screenshot.png)

Instructions
------------

There is a precompiled executable for Windows. To use it, download the content of the `Release` folder and start the executable.

### Key controls

- `Esc` Exit the application.
- `F2` Toggle wire frame mode.
- `F11` Toggle between fullscreen and windowed mode.
- `Tab` Insert new cube at the center.
- `Up`, `Down`, `Left`, `Right` or `W`, `A`, `S`, `D` Move camera.
- `PageUp`, `PageDown` or `Q`, `E` Raise and lower the camera.
- `Space` Let the inserted cubes jump.

Branches
--------

### [Terrain](https://github.com/ComputerGame/GraphicsApplication/tree/terrain)

In this branch a voxel based terrain generator and renderer is implmented.

Upcoming Features
-----------------

There is a lot to do since the core development isn't finished yet.

- Lighting, optimally [global illumination](http://en.wikipedia.org/wiki/Global_illumination)
- Physics and Collision
- Model Loading

Desired features that might be implemented in future.

- Animations
- Characters
- Artificial intelligence
- Networking, for multiplayer
- [Screen space ambient occlusion](http://en.wikipedia.org/wiki/Screen_space_ambient_occlusion)