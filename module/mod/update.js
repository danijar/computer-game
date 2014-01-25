
// move flashlight with player
if (flashlight_on) {
    var coordinates = position(player());
    position(flashlight_id, coordinates[0], coordinates[1] + 1.5, coordinates[2]);
}
