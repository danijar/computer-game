#include "system/system.h"

#include "module/settings/module.h"
#include "module/console/module.h"
#include "module/window/module.h"
#include "module/input/module.h"
#include "module/model/module.h"
#include "module/terrain/module.h"
#include "module/sky/module.h"
#include "module/physics/module.h"
#include "module/person/module.h"
#include "module/camera/module.h"
#include "module/renderer/module.h"
#include "module/interface/module.h"
#include "module/mod/module.h"

using namespace std;


int main()
{
	System World;

	World.Add("settings",  new ModuleSettings() );
	World.Add("console",   new ModuleConsole()  );
	World.Add("window",    new ModuleWindow()   );
	World.Add("input",     new ModuleInput()    );
	World.Add("model",     new ModuleModel()    );
	World.Add("terrain",   new ModuleTerrain()  );
	World.Add("sky",       new ModuleSky()      );
	World.Add("person",    new ModulePerson()   );
	World.Add("camera",    new ModuleCamera()   );
	World.Add("renderer",  new ModuleRenderer() );
	World.Add("physics",   new ModulePhysics()  );
	World.Add("interface", new ModuleInterface());
	World.Add("mod",       new ModuleMod()      );

	World.Init();

	string Message = World.UpdateLoop();
	cout << "System exited with message: " << Message << endl;

	// cin.get();
}
