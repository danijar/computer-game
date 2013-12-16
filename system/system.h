#pragma once

#include <string>
#include <vector>
#include <stdint.h>

#include <dependency/v8/v8.h>

#include "manager/archive/manager.h"
#include "manager/data/manager.h"
#include "manager/entity/manager.h"
#include "manager/event/manager.h"
#include "manager/file/manager.h"
#include "manager/global/manager.h"
#include "manager/log/manager.h"
#include "manager/opengl/manager.h"
#include "manager/script/manager.h"


class Module
{
public:
	void Set(std::string Name, ManagerEvent *Event, ManagerEntity *Entity, ManagerGlobal *Global, ManagerData *Data, v8::Persistent<v8::Context> Context, std::string *Message);
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual ~Module() {};
	std::string Name();
	void Exit(std::string Message);
	ManagerEvent  *Event;
	ManagerEntity *Entity;
	ManagerGlobal *Global;
	ManagerData   *Data;
	ManagerLog    *Log;
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
	System();
	void Init();
	void Add(std::string Name, Module *Module);
	void Remove(std::string Name);
	void Pause(std::string Name);
	void Resume(std::string Name);
	bool Paused(std::string Name);
	bool Update();
	std::string UpdateLoop();
	std::string GetMessage();
private:
	private:
	std::vector<std::tuple<std::string, Module*, bool>> list;
	ManagerEvent  *event;
	ManagerEntity *entity;
	ManagerGlobal *global;
	ManagerData   *data;
	std::string message;
};
