
// move flashlight with player
if (flashlight) {
    var pos = position(player());
    position(flashlight, pos[0], pos[1] + 1.5, pos[2]);
}

// let torches flicker
for (var i = 0; i < torches.length; ++i) {
	var flicker = 2;
	for (var j = 0; j < torches[i].states.length; ++j) {
		flicker += 0.5 * Math.abs(Math.sin(torches[i].states[j].value));
		torches[i].states[j].value += torches[i].states[j].speed;
	}
	radius(torches[i].id, flicker);
}
