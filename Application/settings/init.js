
settings({
    // window
    Fullscreen: [ 'bool',     false                  ],
    Mouse:      [ 'bool',     false                  ],
    Size:       [ 'Vector2u', 800, 600               ],
    Title:      [ 'string',   'Graphics Application' ],
    Position:   [ 'Vector2i', 0, 0                   ],

    // renderer
    Fieldofview:  [ 'float', 45.0   ],
    Wireframe:    [ 'bool',  false  ],
    Verticalsync: [ 'bool',  true   ],
    Viewdistance: [ 'float', 1000.0 ],

    // terrain
    Placedistance: [ 'int',    5       ],
    Chunkdistance: [ 'float',  0.1     ],
    World:         [ 'string', 'world' ],
});
