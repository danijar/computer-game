#include "manager.h"

#include <string>
#include <functional>

#include "helper/debug.h"
#include "helper/file.h"

using namespace std;


/*
 * Connects to a database.
 * If the database specified by the path does not exist, a new one is created.
 * If there is no path specified, the path member will be used.
 */
sqlite3 *ManagerData::Open()
{
	return Open("save/" + name + ".db");
}
sqlite3 *ManagerData::Open(string Path)
{
	// create directory structure if not exist
	HelperFile::Create(Path);

	// open and if necessary create database
	sqlite3 *database = nullptr;
	int result = sqlite3_open_v2(Path.c_str(), &database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	if(result != SQLITE_OK)
		HelperDebug::Fail("data", "connect to database fail");
	return database;
}

/*
 * Closes the connection to a database.
 * The connection handle gets deleted.
 */
bool ManagerData::Close(sqlite3 *Database)
{
	int result = sqlite3_close_v2(Database);
	if(result != SQLITE_OK)
	{
		HelperDebug::Fail("data", "disconnect from database failed");
		return false;
	}
	// delete Connection;
	return true;
}

/*
 * Runs a query on the database and calls the passed callback function once per result row.
 * Returns false on error.
 */
bool ManagerData::Query(sqlite3 *Database, string Sql, function<void(sqlite3_stmt*)> Callback)
{
	sqlite3_stmt *statement;
	sqlite3_prepare_v2(Database, Sql.c_str(), -1, &statement, 0);
	while(int result = sqlite3_step(statement))
	{
		if(result == SQLITE_BUSY)
			continue;
		else if(result == SQLITE_ROW)
			Callback(statement);
		else if(result == SQLITE_DONE)
			break;
		else
		{
			HelperDebug::Fail("data", "query failed with code (" + to_string(result) + ")");
			return false;
		}
	}
	return true;
}

/*
 * Creates a new table if it doesn't already exists.
 * Returns false on error.
 */
bool ManagerData::Table(sqlite3 *Database, string Name, unordered_map<string, string> Fields)
{
	string sql = "CREATE TABLE IF NOT EXISTS " + Name + " (id TEXT PRIMARY KEY";
	for(auto i : Fields)
	{
		sql += ", " + i.first + " " + i.second;
	}
	sql += ")";
	return Query(Database, sql);
}
