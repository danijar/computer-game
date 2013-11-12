
function place_player_on_terrain() {
    if (!player()) return;

    var range = 100;
    var pos = position(player());

    var ground = range;
    while (!block(parseInt(pos[0]), ground, parseInt(pos[2])) && ground > -range)
        --ground;

    if (ground == -range) {
        print("could not find plain terrain to set player on");
        position(player(), pos[0], 0, pos[2]);
    } else {
        position(player(), pos[0], ground + 1.80, pos[2]);
        impulse(player(), 300);
        print("set player on terrain");
    }
}

on("TerrainLoadingFinished", function () {
    if (!person()) return;

    var pos = position(player());
    if (parseInt(pos[0]) == 0 && parseInt(pos[2] == 0))
        place_player_on_terrain();
});

var key_reset_down = false;
