
// flashlight
require('mod/helper/togglekey.js');
var flashlight = false;
togglekey('F', function() {
	flashlight = light(0, 0, 0, 18, 0.8, 0.7, 0.5, 1);
	print('flashlight on');
}, function() {
	remove(flashlight);
	flashlight = false;
	print('flashlight off');
});

// house
require('mod/helper/house.js');
key('HOUSE'.split(''), function() {
	var offset = position(player());
	house(offset[0], offset[1], offset[2]);
});

// hide interface and stuff
function hide() {
	renderpass('preview');
	placemarker();
	interface();
}

// place torch like light
var torches = [];
function torch() {
	var pos = position(player());
	torches.push({
		id: light(pos[0], pos[1], pos[2], 3, 1, 0.7, 0.2, 3),
		states: [
			{ value: 0, speed: 0.1 * (Math.random() / 2 + 0.5), },
			{ value: 0, speed: 0.2 * (Math.random() / 2 + 0.5), },
			{ value: 0, speed: 0.3 * (Math.random() / 2 + 0.5), },
		]
	});
}

// place big light five meters above player for lighting tests
function shine() {
	var pos = position(player());
	return light(pos[0], pos[1] + 5, pos[2], 40, 1, .8, .7, 1);
}

// place three color lights moving in circles
var sparks = [];
function spark() {
	var pos = position(player());
	pos[1] += 2;
	sparks.push({
		id: light(pos[0], pos[1], pos[2], 5, 1, 0, 0, 5),
		state: { origin: pos, value: 0, speed: +0.05 * (Math.random() / 2 + 0.5), },
	});
	sparks.push({
		id: light(pos[0], pos[1], pos[2], 5, 0, 1, 0, 5),
		state: { origin: pos, value: 0, speed: -0.05 * (Math.random() / 2 + 0.5), },
	});
	sparks.push({
		id: light(pos[0], pos[1], pos[2], 5, 0, 0, 1, 5),
		state: { origin: pos, value: 0, speed: +0.05 * (Math.random() / 2 + 0.5), },
	});
}
