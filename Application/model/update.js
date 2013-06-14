
// set place on terrain
var worldheight = 32;
var pos = position(person());
if (pos[1] < 1) {
    var floor;
    for (floor = worldheight - 1; 0 < floor - 1; --floor)
        if (block(parseInt(pos[0]), floor, parseInt(pos[2])))
            break;
    position(person(), pos[0], floor + 1, pos[2]);
    impulse(person(), 1000);
}
