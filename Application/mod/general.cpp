#include "module.h"


void ModuleMod::Init()
{
	Script->Inline("var a = 42; print('inline script like number ' + a);");
	Script->Run("init.js");
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
