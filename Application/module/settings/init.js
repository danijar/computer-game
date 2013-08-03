
settings({
    // window
    Fullscreen: [ 'bool',     false                  ], // Determines whether the application starts in fullscreen more or in windowed mode.
    Mouse:      [ 'bool',     false                  ], // Determines whether the mouse is free or captured by the application to change the view direction.
    Size:       [ 'Vector2u', 800, 600               ], // Initial size of the window.
    Title:      [ 'string',   'Graphics Application' ], // Caption of the window. This will be hard coded later on.
    Position:   [ 'Vector2i', 0, 0                   ], // Window position on screen. Doesn't affect the initial position for now and therefore will maybe be removed later on.

    // renderer
    Fieldofview:  [ 'float', 45.0   ], // Angle of the section your eyes can see. Depends on distance to monitor. High values allow you to have a wider view range but visually distort geometry.
    Wireframe:    [ 'bool',  false  ], // Draw only edges but not faces of models. Only for debug purpose.
    Verticalsync: [ 'bool',  true   ], // Determines whether the frame rate is synchronized with the monitor refresh rate.
    Viewdistance: [ 'float', 1000.0 ], // Currently, a value below 1000.0 would not show the sky anymore.

    // terrain
    Placedistance: [ 'int',    5       ], // Defines how far blocks you can mine and place.
    Chunkdistance: [ 'float',  0.1     ], // Defines how distant chunks are loaded relative to view distance. Depending on your machine a value higher than 0.1 might be very slow.
    World:         [ 'string', 'world' ], // Change this to load another world on startup. Worlds are stored in terrain/save/.

    // development
    Debug: [ 'bool', true ], // Print debug information on screen like frame rate, player position, number of forms, and more.
});
