
/*
 * call function to build a house
 * when the H key gets pressed
 */

var once = true;

if(!key('H'))
	once = true;

if(key('H') && once)
{
	once = false;

	var offset = position(person());
	house(parseInt(offset[0]), parseInt(offset[1]) - 1, parseInt(offset[2]));
}
