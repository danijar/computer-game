#include "module.h"

#include <SQLITE/sqlite3.h>


void ModuleMod::Init()
{
	Script->Run("init.js");

	// testing sqlite
	sqlite3 *database;
	int result = sqlite3_open_v2("path.db", &database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	if(!database || result != SQLITE_OK)
	{
		Debug->Fail("connect to database fail");
		return;
	}
	
	Debug->Pass("connect to database success");

	result = sqlite3_close_v2(database);
	if(result == SQLITE_OK)
	{
		Debug->Fail("disconnect from database success");
	}
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
