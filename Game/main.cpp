#include "main.h"

int main()
{
	System World;

	World.Add("settings", new ComponentSettings(), Input);
	World.Add("window", new ComponentWindow(), Input);
	World.Add("input", new ComponentInput(), Input);
	World.Add("renderer", new ComponentRenderer(), Output);

	World.Init();

	string Message = World.UpdateWhile();
	cout << "System exited with message: " << Message << endl;

	// cin.get();
}