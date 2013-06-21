
/*
 * call function to build a house
 * when the H key gets pressed
 */

if(house_keys_up())
	house_keys_once = true;

if(house_keys_down() && house_keys_once)
{
	house_keys_once = false;

	var offset = position(person());
	house(offset[0], offset[1], offset[2]);
}
