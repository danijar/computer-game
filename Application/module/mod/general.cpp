#include "module.h"


void ModuleMod::Init()
{
	Data->Test();

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
