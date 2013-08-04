
// place player on terrain
var worldheight = 50;
var groundheight = -15;
var pos = position(person());
if (pos[1] < groundheight) {
    var floor;
    for (floor = worldheight - 1; 0 < floor - 1; --floor)
        if (block(parseInt(pos[0]), floor, parseInt(pos[2])))
            break;
    position(person(), pos[0], floor + 3, pos[2]);
    impulse(person(), 1000);
}
