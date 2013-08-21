
function place_player_on_terrain()
{
    if(!person()) return;

    var range = 100;
    var pos = position(person());

    var ground = range;
    while (!block(pos[0], ground, pos[2]) && ground > -range)
        --ground;

    position(person(), pos[0], ground + 3, pos[2]);
    print("set player on terrain");
}

on("TerrainLoadingFinished", function(){
    if(!person()) return;

    var pos = position(person());
    if(pos[0] == 0 && pos[2] == 0)
        place_player_on_terrain()
});
