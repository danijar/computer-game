Graphics Application
====================

This is an OpenGL graphics application using a component and data based [Graphics Architecture](https://github.com/ComputerGame/GraphicsArchitecture) with event manager.

Features
--------

- Camera
- Movement
- Textures

Libraries
---------

- [SFML](http://www.sfml-dev.org/) Multimedia Library (2.0 RC)
- [GLEW](http://glew.sourceforge.net/) OpenGL Extension Wrangler Library (1.9.0)
- [GLM](http://glm.g-truc.net/) OpenGL Mathematics (0.9.4.0)

Screenshot
----------

![Screenshot](https://raw.github.com/ComputerGame/GraphicsApplication/master/screenshot.png)

Instructions
------------

A precompiled executables for Windows is found in the `Debug` folder. You need the `Application.exe` and the `shaders` folder and it's content. You can find that in the `Debug` folder of that project. The executable has all needed libraries included. 

### Key controls

- `Esc` Exit the application.
- `F11` Toggle between fullscreen and windowed mode.
- `Tab` Insert new cube at the center.
- `Up`, `Down`, `Left`, `Right` or `W`, `A`, `S`, `D` Move cubes.
- `PageUp`, `PageDown` or `Q`, `E` Raise and lower the cube.
- `Space` Let the cube jump.

Branches
--------

### [Terrain](https://github.com/ComputerGame/GraphicsApplication/tree/terrain)

In this branch a voxel based terrain generator and renderer is implmented.

Upcoming Features
-----------------

There is a lot to do since the core development isn't finished yet.

- Model Loading
- Scenegraph
- Terrain, a voxel world smoothed by an particular algorithm
- Collision
- Lighting, optimally [global illumination](http://en.wikipedia.org/wiki/Global_illumination)

Desired features that might be implemented in future.

- Animations
- Non person characters
- Artificial intelligence
- Networking, for multiplayer
- [Screen space ambient occlusion](http://en.wikipedia.org/wiki/Screen_space_ambient_occlusion)