#pragma once

#include "system.h"

#include <string>
using namespace std;

#include  "settings/module.h"
#include   "console/module.h"
#include    "window/module.h"
#include     "input/module.h"
#include     "model/module.h"
#include       "sky/module.h"
#include    "physic/module.h"
#include    "person/module.h"
#include    "camera/module.h"
#include  "renderer/module.h"
#include "interface/module.h"
#include       "mod/module.h"


int main()
{
	System World;

	World.Add("settings",  new ModuleSettings() );
	World.Add("console",   new ModuleConsole()  );
	World.Add("window",    new ModuleWindow()   );
	World.Add("input",     new ModuleInput()    );
	World.Add("model",     new ModuleModel()    );
	World.Add("sky",       new ModuleSky()      );
	World.Add("person",    new ModulePerson()   );
	World.Add("camera",    new ModuleCamera()   );
	World.Add("renderer",  new ModuleRenderer() );
	World.Add("physic",    new ModulePhysic()   );
	World.Add("interface", new ModuleInterface());
	World.Add("mod",       new ModuleMod()      );

	World.Init();

	string Message = World.UpdateLoop();
	cout << "System exited with message: " << Message << endl;

	// cin.get();
}
