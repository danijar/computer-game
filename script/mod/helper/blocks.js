
// set a volume of blocks
function blocks(position_x, position_y, position_z, size_x, size_y, size_z, type, condition)
{
	for(var x = position_x; x < position_x + size_x; ++x)
		for(var y = position_y; y < position_y + size_y; ++y)
			for(var z = position_z; z < position_z + size_z; ++z)
				if(condition === undefined ? true : condition(x, y, z))
					block(x, y, z, type);
}
