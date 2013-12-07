
if (!key_reset_down && key('R') && key('E') && key('S') && key('E') && key('T')) {
    key_reset_down = true;

    /*
    if (player()) {
        var pos = position(player());
        position(player(), pos[0], 10, pos[2]);
        print("reset player position");
    }
    */

    if (player()) {
        place_player_on_terrain();
    }
}
else if (key_reset_down && !key('R') && !key('E') && !key('S') && !key('E') && !key('T')) {
    key_reset_down = false;
}
