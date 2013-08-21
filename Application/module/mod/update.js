
if (!keys_house_once && key('H') && key('O') && key('U') && key('S') && key('E')) {
    keys_house_once = true;

    var offset = position(person());
    house(offset[0], offset[1], offset[2]);
}
else if (keys_house_once && !key('H') && !key('O') && !key('U') && !key('S') && !key('E')) {
    keys_house_once = false;
}
