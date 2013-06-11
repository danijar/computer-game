
// set place on terrain
var pos = position(person());
if (pos[1] < 1)
    position(person(), pos[0], 64, pos[2]);
