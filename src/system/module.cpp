#include "system/system.h"

using namespace std;
using namespace v8;

void Module::Set(string Name, ManagerEvent *Event, ManagerEntity *Entity, ManagerGlobal *Global, ManagerData *Data, Persistent<Context, CopyablePersistentTraits<Context>> Context, string *Message)
{
	this->name    = Name;
	this->Event   = Event;
	this->Entity  = Entity;
	this->Global  = Global;
	this->Data    = Data;
	this->Log     = new ManagerLog(Name);
	this->Logic   = new ManagerLogic();
	this->File    = new HelperFile(Name);
	this->Opengl  = new HelperOpengl(Name);
	this->Script  = new HelperScript(Name, this, Context);
	this->Archive = new HelperArchive(Name);
	this->message = Message;
}

string Module::Name()
{
	return name;
}

void Module::Exit(string Message)
{
	*this->message = Message;
}
