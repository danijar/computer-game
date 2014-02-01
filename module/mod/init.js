
/*
 * Flashlight
 */

// globals
var flashlight_on = false;
var flashlight_id = 0;

// key pressed
key('F', function () {
	// light off
	if (flashlight_on) {
		flashlight_on = false;
		remove(flashlight_id);
		print("flashlight off");
	}
	// light on
	else {
		flashlight_on = true;
		flashlight_id = light(0, 0, 0, 18, 0.8, 0.7, 0.5, 3.0);
		print("flashlight on");
	}
});

/*
 * House
 */

require('module/mod/helper/house.js');

// globals
var house_keys_once = true;

// key pressed
on('InputKeyPressed', function () {
	if (!house_keys_once) return;
	house_keys_once = false;

	if (key('H') && key('O') && key('U') && key('S') && key('E')) {
		// build house around the player
	    var offset = position(player());
	    house(offset[0], offset[1], offset[2]);
	}
});

// key released
on('InputKeyReleased', function () {
	if (!key('H') || !key('O') || !key('U') || !key('S') || !key('E')) {
		house_keys_once = true;
	}
});
