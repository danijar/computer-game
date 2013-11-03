#include "module.h"

#include <string>
using namespace std;


void ModuleMod::Init()
{
	/*
	string name = "world";
	Data->Name = name;
	Event->Fire<string>("SavegameChanged", name);

	Data->Test();
	*/
	Script->Run("init.js");
}

void ModuleMod::Update()
{
	Script->Run("update.js");
}

void ModuleMod::Listeners()
{
	Event->Listen("SystemInitialized", [=]{
		/*
		 * scan for mods by listing sub directories
		 * for each mod
		 *    run "init.js" script
		 *    or do more complicated things
		 */
	});
}
