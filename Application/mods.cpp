#pragma once

#include "system.h"


class ModuleMods : public Module
{
	void Init()
	{
		/*
		 * for each sub directory in Name()
		 *     create a new entity
		 *     add mod storage to it
		 */
	}

	void Update()
	{

	}

	void Listeners()
	{
		Event->Listen("SystemInitialized", [=]{
			// Script->Run(Name + "init.js");

			/*
			 * for each mod in entities
			 *    run script or do more complicated things
			 */
		});
	}
};