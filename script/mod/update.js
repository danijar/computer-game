
// move flashlight with player
if (flashlight) {
    var pos = position(player());
    position(flashlight, pos[0], pos[1] + 1.5, pos[2]);
}

// let torches flicker
if (torches) for (var i = 0; i < torches.length; ++i) {
	var states = torches[i].states;
	var flicker = 2;
	for (var j = 0; j < states.length; ++j) {
		var state = states[j];
		flicker += 0.5 * Math.abs(Math.sin(state.value));
		state.value += state.speed;
	}
	radius(torches[i].id, flicker);
}

// move sparks in circles
if (sparks) for (var i = 0; i < sparks.length; ++i) {
	var state = sparks[i].state;
	var range = 2 + Math.sin(0.2 * state.value + 0.3);
	var x = state.origin[0] + range * Math.sin(state.value),
		y = state.origin[1] + range * Math.sin(0.3 * state.value + 0.7),
		z = state.origin[2] + range * Math.cos(state.value);
	state.value += state.speed;
	position(sparks[i].id, x, y, z);
}
