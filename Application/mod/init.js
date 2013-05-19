
/*
 * build a simple house
 */
 
function house(offset_x, offset_y, offset_z, half_length)
{
	var half_length = half_length || 4;

	// make sure ground is solid
	for(var x = -half_length + offset_x; x <= half_length + offset_x; ++x)
		for(var z = -half_length + offset_z; z <= half_length + offset_z; ++z)
			for(var y = 0; y <= offset_y; ++y)
				if(!block(x, y, z))
					block(x, y, z, Math.random() > 0.5 ? 1 : 2);

	// build walls
	for(var x = -half_length + offset_x; x <= half_length + offset_x; ++x)
	{
		for(var y = offset_y; y <= half_length + offset_y; ++y)
			block(x, y, offset_z + half_length, 1);
		for(var y = offset_y; y <= half_length + offset_y; ++y)
			block(x, y, offset_z - half_length, 1);
	}
	for(var z = -half_length + offset_z; z <= half_length + offset_z; ++z)
	{
		for(var y = offset_y; y <= half_length + offset_y; ++y)
			block(offset_x + half_length, y, z, 1);
		for(var y = offset_y; y <= half_length + offset_y; ++y)
			block(offset_x - half_length, y, z, 1);
	}

	// free space inside
	for(var x = -half_length + 1 + offset_x; x <= half_length - 1+ offset_x; ++x)
		for(var z = -half_length + 1 + offset_z; z <= half_length - 1+ offset_z; ++z)
			for(var y = offset_y + 1; y <= offset_y + half_length; ++y)
				block(x, y, z, 0);
}
