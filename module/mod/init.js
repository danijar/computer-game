
/*
 * Flashlight
 */

// globals
var flashlight_keys_once = true;
var flashlight_on = false;
var flashlight_id = 0;

// key pressed
on('InputKeyPressed', function () {
	if (!flashlight_keys_once) return;
	flashlight_keys_once = false;

	if (key('F')) {
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
	}
});

// key released
on('InputKeyReleased', function () {
	if (!key('F')) {
		flashlight_keys_once = true;
	}
});

/*
 * House
 */

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

// build a nice house
function house(position_x, position_y, position_z, size_x, size_y, size_z)
{
	// parameterss
	size_x = parseInt(size_x || (Math.floor(5 + Math.random() * 6)) / 2) * 2;
	size_y = parseInt(size_y || (Math.floor(4 + Math.random() * 2))    )    ;
	size_z = parseInt(size_z || (Math.floor(5 + Math.random() * 6)) / 2) * 2;

	var origin_x = parseInt(position_x - size_x / 2),
		origin_y = parseInt(position_y),
		origin_z = parseInt(position_z - size_z / 2);

	var orientation = (size_x > size_z);

	// materials
	var brick = 6,
		roof  = 3
		floor = 4;

	// fundament
	blocks(origin_x - 1, 0, origin_z - 1, size_x + 2, origin_y, size_z + 2, 2, function(x, y, z){ return block(x, y, z) == 0; });

	// get some space
	blocks(origin_x - 1, origin_y, origin_z - 1, size_x + 2, 1, size_z + 2, 0);
	blocks(origin_x - 2, origin_y + 1, origin_z - 2, size_x + 4, size_y + 1, size_z + 4, 0);

	// walls
	blocks(origin_x,     origin_y - 1, origin_z,     size_x,     size_y + 1, size_z,     brick);
	blocks(origin_x + 1, origin_y,     origin_z + 1, size_x - 2, size_y - 1, size_z - 2, 0);

	// floor (optional)
	blocks(origin_x + 1, origin_y - 1, origin_z + 1, size_x - 2, 1, size_z - 2, floor);

	// door
	var side = Math.floor(Math.random() * 3) + 1;
	if(side == 1) {
		blocks(origin_x + size_x/2 - 1, origin_y, origin_z, 2, 2, 1, 0);
		blocks(origin_x + size_x/2 - 1, origin_y + 1, origin_z + size_z - 1, 2, 1, 1, 0);
	}
	else if(side == 2) {
		blocks(origin_x, origin_y, origin_z + size_z/2 - 1, 1, 2, 2, 0);
		blocks(origin_x + size_x - 1, origin_y + 1, origin_z + size_z/2 - 1, 1, 1, 2, 0);
	}
	else if(side == 3) {
		blocks(origin_x + size_x/2 - 1, origin_y, origin_z + size_z - 1, 2, 2, 1, 0);
		blocks(origin_x + size_x/2 - 1, origin_y + 1, origin_z, 2, 1, 1, 0);
	}
	else if(side == 4) {
		blocks(origin_x + size_x - 1, origin_y, origin_z + size_z/2 - 1, 1, 2, 2, 0);
		blocks(origin_x, origin_y + 1, origin_z + size_z/2 - 1, 1, 1, 2, 0);
	}

	// gable
	if(orientation == true)
		for(var i = 1; i < size_z - 1; ++i) {
			blocks(origin_x,              origin_y + size_y, origin_z + i, 1, 1, 1, brick);
			blocks(origin_x + size_x - 1, origin_y + size_y, origin_z + i, 1, 1, 1, brick);
		}
	else
		for(var i = 1; i < size_x - 1; ++i) {
			blocks(origin_x + i, origin_y + size_y, origin_z,              1, 1, 1, brick);
			blocks(origin_x + i, origin_y + size_y, origin_z + size_z - 1, 1, 1, 1, brick);
		}

	// roof
	if(orientation == true)
	{
		for(var i = 0; i < size_z / 2 + 1 && i < 3; ++i) {
			blocks(origin_x - 1, origin_y - 2 + size_y + i, origin_z - 1      + i, size_x + 2, 1, 1, roof);
			blocks(origin_x - 1, origin_y - 2 + size_y + i, origin_z + size_z - i, size_x + 2, 1, 1, roof);
		}
		for(var i = 2; i < size_z - 2; ++i)
			blocks(origin_x - 1, origin_y + size_y + 1, origin_z + i, size_x + 2, 1, 1, roof);
	}
	else
	{
		for(var i = 0; i < size_x / 2 + 1 && i < 3; ++i) {
			blocks(origin_x - 1      + i, origin_y - 2 + size_y + i, origin_z - 1, 1, 1, size_z + 2, roof);
			blocks(origin_x + size_x - i, origin_y - 2 + size_y + i, origin_z - 1, 1, 1, size_z + 2, roof);
		}
		for(var i = 2; i < size_x - 2; ++i)
			blocks(origin_x + i, origin_y + size_y + 1, origin_z - 1, 1, 1, size_z + 2, roof);
	}

	// light
	light(position_x, position_y + size_y/2 - 1, position_z, 4 * size_y, 1, 1, 1, 1.2);
}

// helpers
function blocks(position_x, position_y, position_z, size_x, size_y, size_z, type, condition)
{
	for(var x = position_x; x < position_x + size_x; ++x)
		for(var y = position_y; y < position_y + size_y; ++y)
			for(var z = position_z; z < position_z + size_z; ++z)
				if(condition === undefined ? true : condition(x, y, z))
					block(x, y, z, type);
}
