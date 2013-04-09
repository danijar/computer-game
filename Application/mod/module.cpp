#include "module.h"


void ModuleMod::Init()
{

}

void ModuleMod::Update()
{

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
