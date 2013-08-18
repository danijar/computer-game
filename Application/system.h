#pragma once

#include <string>
#include <vector>

#include "manager/event.h"
#include "manager/entity.h"
#include "manager/global.h"
#include "manager/data.h"

#include "helper/debug.h"
#include "helper/file.h"
#include "helper/opengl.h"
#include "helper/script.h"
#include "helper/archive.h"


class Module
{
public:
	void Set(std::string Name, ManagerEvent *Event, ManagerEntity *Entity, ManagerGlobal *Global, ManagerData *Data, v8::Persistent<v8::Context> Context, std::string *Message)
	{
		this->name    = Name;
		this->Event   = Event;
		this->Entity  = Entity;
		this->Global  = Global;
		this->Data    = Data;
		this->Debug   = new HelperDebug(Name);
		this->File    = new HelperFile(Name);
		this->Opengl  = new HelperOpengl(Name);
		this->Script  = new HelperScript(Name, this, Context);
		this->Archive = new HelperArchive(Name);
		this->message = Message;
	}
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual ~Module() {};
	std::string Name()
	{
		return name;
	}
	void Exit(std::string Message)
	{
		*this->message = Message;
	}
	ManagerEvent  *Event;
	ManagerEntity *Entity;
	ManagerGlobal *Global;
	ManagerData   *Data;
	HelperDebug   *Debug;
	HelperFile    *File;
	HelperOpengl  *Opengl;
	HelperScript  *Script;
	HelperArchive *Archive;
private:
	std::string name;
	std::string *message;
};


class System
{
public:
	System()
	{
		event   = new ManagerEvent();
		entity  = new ManagerEntity();
		global  = new ManagerGlobal();
		data    = new ManagerData();
		message = "";
	}

	void Init()
	{
		v8::Persistent<v8::Context> context = v8::Context::New();
		context->Enter();
		for (auto i : list)
		{
			std::get<1>(i)->Set(std::get<0>(i), event, entity, global, data, context, &message);
			std::get<1>(i)->Init();
		}
		event->Fire("SystemInitialized");
	}

	void Add(std::string Name, Module *Module)
	{
		for(auto i : list)
		{
			if(std::get<0>(i) == Name)
			{
				HelperDebug::Crash("system", "cannot add module (" + Name + ") because the name already exists");
				return;
			}
		}
		list.push_back(make_tuple(Name, Module, true));
	}
	void Remove(std::string Name)
	{
		for(auto i = list.begin(); i != list.end(); ++i)
		{
			if(std::get<0>(*i) == Name)
			{
				list.erase(i);
				return;
			}
		}
		HelperDebug::Warning("system", "cannot remove module (" + Name + ") because the name was not found");
	}
	void Pause(std::string Name)
	{
		for (auto i : list)
		{
			if(std::get<2>(i) == false) HelperDebug::Warning("system", "module (" + Name + ") is already paused");
			else std::get<2>(i) = false;
			return;
		}
		HelperDebug::Warning("system", "cannot pause module (" + Name + ") because the name was not found");
	}
	void Resume(std::string Name)
	{
		for (auto i : list)
		{
			if(std::get<2>(i) == true) HelperDebug::Warning("system", "module (" + Name + ") isn't paused");
			else std::get<2>(i) = true;
			return;
		}
		HelperDebug::Warning("system", "cannot resume module (" + Name + ") because the name was not found");
	}
	bool Paused(std::string Name)
	{
		for (auto i : list)
		{
			return !std::get<2>(i);
		}
		HelperDebug::Warning("system", "cannot find module (" + Name + ")");
	}
	bool Update()
	{
		for(auto i : list)
		{
			if(message != "") return false;
			if(std::get<2>(i))
				std::get<1>(i)->Update();
		}
		event->Fire("SystemUpdated");
		return true;
	}
	std::string UpdateLoop()
	{
		while(Update());
		return message;
	}
	std::string GetMessage()
	{
		return message;
	}
private:
	private:
	std::vector<std::tuple<std::string, Module*, bool>> list;
	ManagerEvent  *event;
	ManagerEntity *entity;
	ManagerGlobal *global;
	ManagerData   *data;
	std::string message;
};
