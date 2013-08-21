
if (!key_reset_down && key('R') && key('E') && key('S') && key('E') && key('T')) {
    key_reset_down = true;

    if (person()) {
        var pos = position(person());
        position(person(), pos[0], 10, pos[2]);
        print("reset player position");
    }
}
else if (key_reset_down && !key('R') && !key('E') && !key('S') && !key('E') && !key('T')) {
    key_reset_down = false;
}
