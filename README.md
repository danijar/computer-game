Graphics Application: Terrain
=============================

This is a branch of a basic [Graphics Application](https://github.com/ComputerGame/GraphicsApplication) extending the functionality by voxel based terrain capabilities.

Features
--------

- Random terrain generation using [Simplex noise](http://en.wikipedia.org/wiki/Simplex_noise)
- Culled mesh generation from voxel data
- Different block looks using a texture atlas
- Fog fading out far landscapes

Screenshot
----------

![Screenshot](https://raw.github.com/ComputerGame/GraphicsApplication/terrain/screenshot.png)

Instructions
------------

There is a precompiled executable for Windows. To use it, download the content of the `Application` folder and run the executable. You may need a recent Windows version to run this.

Also see the [key controls](https://github.com/ComputerGame/GraphicsApplication/#key-controls) of the main application.

Upcoming Features
-----------------

- Generation of more simple meshes with adjacent face merging
- Block modification at runtime
- Terrain smoothing by a particular algorithm
