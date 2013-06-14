
/*
 * call function to build a house
 * when the H key gets pressed
 */

if(!key('H'))
	once = true;

if(key('H') && once)
{
	once = false;

	var offset = position(person());
	house(offset[0], offset[1], offset[2]);
}
