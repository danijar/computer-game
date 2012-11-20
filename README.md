Graphics Application
====================

This is an OpenGL graphics application using a component and data based [Graphics Architecture](https://github.com/ComputerGame/GraphicsArchitecture) with event manager.

Libraries
---------

- [SFML](http://www.sfml-dev.org/) Multimedia Library (2.0 RC)
- [GLEW](http://glew.sourceforge.net/) OpenGL Extension Wrangler Library (1.9.0)
- [GLM](http://glm.g-truc.net/) OpenGL Mathematics (0.9.4.0)

Screenshot
----------

This cube is all you can see for now.

[![Screenshot](http://danijar.org/wp-content/uploads/graphicsapplicationcube.png)](http://danijar.org/wp-content/uploads/graphicsapplicationcube.png)

Instructions
------------

A precompiled executables for Windows is found in the `Debug` folder. You only need the `Application.exe`, it is standalone and has all needed libraries included.

###Key controls

- Space. The cube will do a little jump.
- Esc. Exit the application.
- F11. Toggle between fullscreen and windowed mode.

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