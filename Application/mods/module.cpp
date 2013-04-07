#include "module.h"


void ModuleMods::Init()
{

}

void ModuleMods::Update()
{

}

void ModuleMods::Listeners()
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
