#include "system/system.h"

using namespace std;
using namespace v8;


System::System()
{
	event   = new ManagerEvent();
	entity  = new ManagerEntity();
	global  = new ManagerGlobal();
	data    = new ManagerData();
	message = "";
}

void System::Init()
{
	Isolate *isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	Local<Context> context = Context::New(isolate);
	context->Enter();

	for (auto i : list) {
		get<1>(i)->Set(get<0>(i), event, entity, global, data, context, &message);
		get<1>(i)->Init();
	}

	event->Fire("SystemInitialized");
}

void System::Add(string Name, Module *Module)
{
	for (auto i : list) {
		if (get<0>(i) == Name) {
			ManagerLog::Crash("system", "cannot add module (" + Name + ") because the name already exists");
			return;
		}
	}
	list.push_back(make_tuple(Name, Module, true));
}

void System::Remove(string Name)
{
	for (auto i = list.begin(); i != list.end(); ++i) {
		if (get<0>(*i) == Name) {
			list.erase(i);
			return;
		}
	}
	ManagerLog::Warning("system", "cannot remove module (" + Name + ") because the name was not found");
}

void System::Pause(string Name)
{
	for (auto i : list) {
		if (get<2>(i) == false)
			ManagerLog::Warning("system", "module (" + Name + ") is already paused");
		else
			get<2>(i) = false;
		return;
	}
	ManagerLog::Warning("system", "cannot pause module (" + Name + ") because the name was not found");
}

void System::Resume(string Name)
{
	for (auto i : list) {
		if (get<0>(i) == Name) {
			if (get<2>(i) == true)
				ManagerLog::Warning("system", "module (" + Name + ") isn't paused");
			else
				get<2>(i) = true;
			return;
		}
	}
	ManagerLog::Warning("system", "cannot resume module (" + Name + ") because the name was not found");
}

bool System::Paused(string Name)
{
	for (auto i : list)
		if (get<0>(i) == Name)
			return !get<2>(i);
	ManagerLog::Warning("system", "cannot find module (" + Name + ")");
	return false;
}

bool System::Update()
{
	for (auto i : list) {
		if (message != "")
			return false;
		// cout << "Update " << get<0>(i) << endl;
		if (get<2>(i))
			get<1>(i)->Update();
	}
	event->Fire("SystemUpdated");
	return true;
}

string System::UpdateLoop()
{
	while(Update());
	return message;
}

string System::GetMessage()
{
	return message;
}
