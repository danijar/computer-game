Ideas
=====

- Cache for persons whether they stay on ground
- Improve Data Manager
	- speed up by reusing compiled prepared statements
	- provide functions for asynchron saving and loading
	- make code more intuitive by making use of internal Query() function
- Meta module system
	- loads in all available DLL files
	- adds them to the world with their file name as name
- Speed up terrain
	- reuse the large voxel arrays of discarded chunks (kind of array pool)
- Digging animation
- Optimize rendering pipeline
	- reduce gbuffer to two or three textures
	- maybe reuse textures
- Get id of focused object
	- cast ray and compare result against all forms to get id
- Treat managers and helpers the same
	- http://ideone.com/csNMx5
- Create asset manager
	- move functionality out of Model module
	- generalize for all types
	- live reload using Hash() from File Manager
- Add scripting callbacks to toggle modules by their name
	- provide Pause() and Resume() function for modules to override
- Readd forward renderer as alternative
- Live reload for scripts
	- move file hash function into a file helper
	- maybe even store scripts in asset manager
- Improve Texture Mapping
	- fallback to 1x1 diffuse map with pink color
	- added texture tinting through color vertex attributes
- Support camera rotation along all axis, even roll
- When mouse is free, select block unter cursor instead of in the middle of screen
- Speed up lighting
	- Draw bounding geometry for point lights to cull not affected pixels
	- support for spot lights
- Implement Window as entity (do this later and mainly for consistency)
	- therefore support multiple windows
	- each windows draws the scene from the camera entity it's bound to
	- store active window id globally instead of active camera id
	- should be fire resize and recreated events from all windows?
		- maybe yes, and make also sure that the related window is set to active before
		- so that viewport and shader uniforms are set for the right window
			- are they window dependent?
- Improve terrain generation
	- load terrain parameters from scripts
	- implement live terrain reload on parameter change
	- retweak whole generation
- Overthink chunk loading mechanism
	- optimize current ellipsoid range
	- smoothly fade in newly loaded chunks and fade out freed ones
	- simpler geometry for far horizons
		- http://www.sea-of-memes.com/LetsCode28/LetsCode28.html
- Frustum culling on a per model basis
	- save expanse of models in all six directions from origin
	- skip models out of view in renderer based on position and expanse
	- http://www.iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
- Implement Artificial Intelligence
	- add capsule primitive and attach to persons
	- store bahavior queues attached to persons
	- store AI and its state in a Thought property attached to persons
	- load different AIs from scripts
- Implement sky system
	- Color skydome per vertex
	- Render sun as alphablended texture projected onto the skydome
	- Make sun moving by time and update light source accordingly
	- Implement procedural clouds projected onto the skydome
	- Advanced fog
		- http://www.iquilezles.org/www/articles/fog/fog.htm
	- Day and night cycles
		- change skydome color
		- move sun and moon
		- how to implement stars for night skyies?
- Implement temporal anti aliasing
	- store velocity vector in gbuffer
- Implement light scattering ("god rays")
- Implement in game profiling and benchmarking
- Run scripts in namespaces named by their modules
	- Modules inject their callback functions as properties of a global javascript object of their name
- Add audio manager
	- sound occlusion, decrease radius and add high pass filter
		- use as input for artificial intelligence
- Mesh Collision Shapes
	- Determine whether a mesh is convex or concave
		- http://liam.flookes.com/cs/geo/index.html
	- Convex meshes use btConvexHull
		- simplify by BtShapeHull vertex reduction utility
	- Concave static meshes use btBvhTriangleMeshShape
	- Concave dynamic meshes use btCompoundShape containing many btConvexHull
		- generated by HACD
		- afterwards maybe simplify by BtShapeHull vertex reduction utility
	- Implement anti tunneling:
		- http://bulletphysics.org/mediawiki-1.5.8/index.php/Anti_tunneling_by_Motion_Clamping
- Let dynamic voxels snap back into stadtic terrain grid
	- do this smoothly by applying linear and radial forces from the grids
	- example without forces
		- http://www.youtube.com/watch?v=ptC4LUdZ8n8
- Support transparent textures
	- either read alpha channel or use a additional texture file
	- initially, only support full or none transparency
	- compare different approaches for semi transparency in computer graphics
- Change from placeholder capsule to ragdoll characters
- Implement character animations
	- Inspiration
		- Euphoria Natural Motion: http://www.youtube.com/watch?v=wKLaMN9dnjQ
		- Wolfire Overgrowth partial animation blending: http://www.youtube.com/watch?v=zVWZbxGFGiA
- Add modding support
	- think about splitting modules into DLL files, main executable scans folders and loads their DLL files
	- or instead just allow smaller modding through a Mod module
- Set up a project website
	- buy the domain www.opengp.org which reads "Open Game Project"
- Implement in game bug report tool
	- send screenshot, hardware configuration, time and an optional user comment to the server
- Logging system
	- automatedly assign colors to output sources to highlight their messages
	- provide options for solo or mute output sources
- Officially announce project
- Ingame screen capturing
	- images in jpg or webp and clips in h.264 encoded mp4 or vp9 encoded webm
- Improve terrain meshing and texturing
	- allow different textures for each side of a block
	- consider generally supporting texture arrays in the engine and update terrain module to use that
- Editor capabilities
	- insert, edit and transform objects
		- inserted objects can get their id stored in a scripting variable or be anonym
	- store scenes in files
		- creating scripting commands as blueprint for the world would be cool
	- allow to seamlessly switch between game and editor mode
	- support resetting the world on switch
- Implement water system
	- maybe using metaballs of varying size depending on the amount of water and or distance to player
		- http://pub.ist.ac.at/group_wojtan/projects/2013_Ando_HALSoTM/index.html
- Dirt layer using multi texturing
	- Crytek's dirt layer:
	- http://freesdk.crydev.net/display/SDKDOC3/Dirt+Layer
- Vegetation system
	- support for normals grass, fern and more
	- maybe procedurally generate meshes and or textures
	- or use prebuilt meshes
	- bend vegetation by contacting forms
- Dynamic destruction, for example of vegetation
- Weather system, affects multiple systems
	- wet shining, wind flackering, bending trees
	- clouds, lighting, tonemapping
- 3d decals like footsteps
- Wind from weather system effects water wave size and direction
- Smooth voxel landscapes by particular algorithm
- Support dynamical rounded blocks, e.g. for tree trunks
	- detect neighbours and join shapes
