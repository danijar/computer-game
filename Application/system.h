#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional> 
#include <memory>
#include <typeindex>
#include <iostream>
#include <fstream>

#include <GLEW/glew.h>
#include <V8/v8.h>


///////////////////////////////////////////////////////////
// prototypes
///////////////////////////////////////////////////////////

class Module;


///////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////

//
// helper debug
//

class HelperDebug
{
public:
	HelperDebug(std::string Name) : name(Name) {}

	void Print(std::string Message)
	{
		Print(name, Message);
	}
	void Pass(std::string Message)
	{
		Pass(name, Message);
	}
	void Fail(std::string Message)
	{
		Fail(name, Message);
	}
	void PassFail(std::string Message, bool Result, std::string Pass = "success", std::string Fail = "fail")
	{
		PassFail(name, Message, Result, Pass, Fail);
	}
	void Warning(std::string Message)
	{
		Warning(name, Message);
	}
	void Wait(std::string Message = "...")
	{
		Wait(name, Message);
	}
	void Crash(std::string Message)
	{
		Crash(name, Message);
	}

	static void Print(std::string Name, std::string Message)
	{
		std::cout << Name << (Name == "" ? "" : " " ) << Message << std::endl;
	}
	static void Inline(std::string Message)
	{
		std::cout << " " << Message;
	}
	static void Pass(std::string Name, std::string Message)
	{
		Print(Name, Message);
	}
	static void Fail(std::string Name, std::string Message)
	{
		Print(Name, Message);
	}
	static void PassFail(std::string Name, std::string Message, bool Result, std::string Pass = "success", std::string Fail = "fail")
	{
		if(Result) HelperDebug::Pass(Name, Message + " " + Pass);
		else       HelperDebug::Fail(Name, Message + " " + Fail);
	}
	static void Warning(std::string Name, std::string Message)
	{
		Print(Name, Message);
	}
	static void Wait(std::string Name, std::string Message = "...")
	{
		Print(Name, Message);
		std::cin.clear();
		std::cin.get();
	}
	static void Crash(std::string Name, std::string Message)
	{
		Wait(Name, "crashed");
		std::exit(EXIT_FAILURE);
	}
private:
	std::string name;
};


//
// helper file
//

class HelperFile
{
public:
	HelperFile(std::string Name) : name(Name) {}

	std::string Read(std::string Path)
	{
		return Read("module/" + name, Path);
	}
	void Write(std::string Path, std::string Text)
	{
		Write("module/" + name, Path, Text);
	}

	static void Write(std::string Name, std::string Path, std::string Text)
	{
		std::string path = Name + "/" + Path;
		// write here
	}
	static std::string Read(std::string Name, std::string Path)
	{
		std::string path = Name + "/" + Path;
		std::ifstream stream(path);
		if(!stream.is_open())
		{
			HelperDebug::Fail(Name, "cannot read file (" + Path + ")");
			return "";
		}
		return std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	}
private:
	std::string name;
};


//
// helper opengl
//

class HelperOpengl
{
public:
	HelperOpengl(std::string Name) : name(Name) {}

	bool Init()
	{
		return Init(name);
	}
	int Test()
	{
		return Test(name);
	}

	static bool Init(std::string Name)
	{
		bool result = (glewInit() == GLEW_OK);
		HelperDebug::PassFail(Name, "glew initialization", result);
		return result;
	}
	static int Test(std::string Name)
	{
		GLenum result;
		int count;
		std::string message;

		for(count = 0; (result = glGetError()) != GL_NO_ERROR; ++count)
		{
			     if(result == GL_INVALID_ENUM)                      message = "GL_INVALID_ENUM";
			else if(result == GL_INVALID_VALUE)                     message = "GL_INVALID_VALUE";
			else if(result == GL_INVALID_OPERATION)                 message = "GL_INVALID_OPERATION";
			else if(result == GL_STACK_OVERFLOW)                    message = "GL_STACK_OVERFLOW";
			else if(result == GL_STACK_UNDERFLOW)                   message = "GL_STACK_UNDERFLOW";
			else if(result == GL_OUT_OF_MEMORY)                     message = "GL_OUT_OF_MEMORY";
			else if(result == GL_INVALID_FRAMEBUFFER_OPERATION_EXT) message = "GL_INVALID_FRAMEBUFFER_OPERATION_EXT";

			HelperDebug::Fail("", message);
		}
		if(count > 0) HelperDebug::Fail(Name, std::to_string(count) + " OpenGL error" + (count > 1 ? "s" : ""));

		return count;
	}
private:
	std::string name;
};


//
// helper script
//

class HelperScript
{
public:
	HelperScript(std::string Name, Module *Module, v8::Persistent<v8::Context> Context) : name(Name), context(Context)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);

		v8::Local<v8::External> handle = v8::External::New(reinterpret_cast<void*>(Module));
		module = v8::Persistent<v8::External>::New(isolate, handle);
	}
	~HelperScript()
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);
		context->Exit();
	}

	void Bind(std::string Name, std::function<v8::Handle<v8::Value>(v8::Arguments const &)> Function)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);

		v8::InvocationCallback *function = Function.target<v8::InvocationCallback>();
		v8::Local<v8::Object> global = context->Global();
		global->Set(v8::String::New(Name.c_str()), v8::FunctionTemplate::New(*function, module)->GetFunction(), v8::ReadOnly);
	}

	bool Load(std::string Path)
	{
		if(scripts.find(Path) != scripts.end())
		{
			HelperDebug::Warning("system", "the script (" + Path + ") is already loaded");
			return true;
		}

		std::string source = HelperFile::Read("module/" + name, Path);
		v8::Persistent<v8::Script> script = Compile(source);
		if(script.IsEmpty())
		{
			HelperDebug::Warning("system", "the script (" + Path + ") cannot be compiled");
			return false;
		}
		
		scripts.insert(std::make_pair(Path, script));
		return true;
	}

	v8::Persistent<v8::Value> Run(std::string Path)
	{
		if(scripts.find(Path) == scripts.end())
			if(!Load(Path))
				return v8::Persistent<v8::Value>(v8::Undefined());

		return Execute(scripts[Path]);
	}

	v8::Persistent<v8::Value> Inline(std::string Source)
	{
		v8::Persistent<v8::Script> script = Compile(Source);
		if(script.IsEmpty())
		{
			HelperDebug::Warning("system", "inline script cannot be compiled");
			return v8::Persistent<v8::Value>(v8::Undefined());
		}

		return Execute(script);
	}

	static Module *Unwrap(v8::Local<v8::Value> Data)
	{
		if(Data.IsEmpty())
		{
			HelperDebug::Fail("", "cannot get module from script argument because it is empty");
			return nullptr;
		}
		else if(!Data->IsExternal())
		{
			HelperDebug::Fail("", "cannot get module from script argument because it's a wrong type");
			return nullptr;
		}

		v8::External *handle = v8::External::Cast(*Data);
		return static_cast<Module*>(handle->Value());
	}

private:
	std::string name;
	v8::Persistent<v8::Context> context;
	std::unordered_map<std::string, v8::Persistent<v8::Script> > scripts;
	v8::Persistent<v8::External> module; // this is only for binding scripts

	v8::Persistent<v8::Script> Compile(std::string Source)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);

		v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::New(Source.c_str()));
		return v8::Persistent<v8::Script>::New(isolate, script);
	}

	v8::Persistent<v8::Value> Execute(v8::Persistent<v8::Script> Script, std::string Name = "")
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);

		v8::TryCatch trycatch;
		v8::Local<v8::Value> result = Script->Run();
		if(result.IsEmpty())
		{
			v8::Handle<v8::Value> exception = trycatch.Exception();
			v8::String::AsciiValue message(exception);
			if(Name == "")
				HelperDebug::Fail("system", "script crashed:");
			else if(Name == "inline")
				HelperDebug::Fail("system", "inline script crashed:");
			else
				HelperDebug::Fail("system", "script (" + Name + ") crashed:");
			HelperDebug::Inline(std::string(*message) + "\n");
			return v8::Persistent<v8::Value>(v8::Undefined());
		}
		return v8::Persistent<v8::Value>::New(isolate, result);
	}
};


///////////////////////////////////////////////////////////
// Managers
///////////////////////////////////////////////////////////

//
// manager event
//

class ManagerEvent
{
	typedef std::unordered_map<std::string, std::unordered_map<int, std::vector<std::pair<void*, bool>>>> Events;
public:
	void Listen(std::string Name, std::function<void()> Function)
	{
		Listen(Name, 0, Function);
	}
	void Listen(std::string Name, int State, std::function<void()> Function)
	{
		list[Name][State].push_back(std::make_pair(new std::function<void()>(Function), false));
	}
	template <typename T>
	void Listen(std::string Name, std::function<void(T)> Function)
	{
		Listen<T>(Name, 0, Function);
	}
	template <typename T>
	void Listen(std::string Name, int State, std::function<void(T)> Function)
	{
		list[Name][State].push_back(std::make_pair(new std::function<void(T)>(Function), true));
	}
	void Fire(std::string Name)
	{
		Fire(Name, 0);
	}
	void Fire(std::string Name, int State)
	{
		auto Functions = list[Name][State];

		for (auto i = Functions.begin(); i != Functions.end(); ++i)
		{
			if(i->second) continue;
			else (*(std::function<void()>*)(i->first))();
		}
	}
	void FireRange(std::string Name, int From, int To)
	{
		for(int i = From; i <= To; ++i) Fire(Name, i);
	}
	template <typename T>
	void Fire(std::string Name, T Data)
	{
		Fire(Name, 0, Data);
	}
	template <typename T>
	void Fire(std::string Name, int State, T Data)
	{
		auto Functions = list[Name][State];

		for (auto i = Functions.begin(); i != Functions.end(); ++i)
		{
			if(i->second) (*(std::function<void(T)>*)i->first)(Data);
			else          (*(std::function<void()>*)i->first)();
		}
	}
private:
	Events list;
};


//
// manager entity
//

class ManagerEntity
{
public:
	ManagerEntity() : index(0) {}
	int New()
	{
		return ++index;
	}
	template <typename T>
	T *Add(unsigned int Id)
	{
		return Add(Id, new T());
	}
	template <typename T>
	T *Add(unsigned int Id, T *Instance)
	{
		if(Id > index || 1 > Id) HelperDebug::Crash("system", "cannot add entity (" + std::to_string(Id) + ") because it is not an entity id. Use 'int New()'.");
		auto key = std::type_index(typeid(T));

		if (list.find(key) == list.end())
		{
			auto value = new std::unordered_map<int, std::shared_ptr<void> >();
			list.insert(make_pair(key, *value));
		}

		auto result = list[key].insert(make_pair(Id, std::shared_ptr<void>(Instance)));
		if (!result.second)
		{
			HelperDebug::Warning("system", "cannot add entity (" + std::to_string(Id) + ") to (" + std::string(key.name()) + ") because it already exists.");
			return Get<T>(Id);
		}
		return Instance;
	}
	template <typename T>
	std::unordered_map<int, T*> Get()
	{
		std::unordered_map<int, T*> output;
		auto key = std::type_index(typeid(T));
		if (Check(key))
		{
			for(auto i = list[key].begin(); i != list[key].end(); ++i)
			{
				output.insert(std::make_pair(i->first, static_cast<T*>(i->second.get())));
			}
		}
		return output;
	}
	template <typename T>
	T *Get(unsigned int Id)
	{
		auto key = std::type_index(typeid(T));
		if (!Check(key, Id))
		{
			HelperDebug::Crash("system", "cannot get entity because (" + std::to_string(Id) + ") in (" + std::string(key.name()) + ") does not exist.");
			return nullptr;
		}
		else return static_cast<T*>(list[key][Id].get());
	}
	void Delete(unsigned int Id)
	{
		if(Id > index || 1 > Id)
		{
			HelperDebug::Warning("system", "cannot delete entity (" + std::to_string(Id) + ") because it is not an entity id.");
			return;
		}
		for(auto i = list.begin(); i != list.end(); ++i)
		{
			if (Check(i->first, Id))
			{
				auto j = i->second.find(Id);
				j->second.reset();
				i->second.erase(j);
			}
		}
	}
	template <typename T>
	void Delete(unsigned int Id)
	{
		auto key = std::type_index(typeid(T));
		if (!Check(key, Id))
		{
			HelperDebug::Crash("system", "cannot delete entity because (" + std::string(key.name()) + ") in (" + std::string(key.name()) + ") does not exist.");
			return;
		}
		auto j = list[key].find(Id);
		j->second.reset();
		list[key].erase(j);
	}
	template <typename T>
	bool Check(unsigned int Id)
	{
		auto key = std::type_index(typeid(T));
		if (!Check(key)) return false;
		if (list[key].find(Id) == list[key].end()) return false;
		return true;
	}
private:
	unsigned int index;
	std::unordered_map<std::type_index, std::unordered_map<int, std::shared_ptr<void>>> list;
	bool Check(std::type_index key)
	{
		if (list.find(key) == list.end()) return false;
		return true;
	}
	bool Check(std::type_index key, int id)
	{
		if (!Check(key)) return false;
		if (list[key].find(id) == list[key].end()) return false;
		return true;
	}
};


//
// manager global
//

class ManagerGlobal
{
public:
	template <typename T>
	T *Add(std::string Name)
	{
		return Add(Name, new T());
	}
	template <typename T>
	T *Add(std::string Name, T *Instance)
	{
		auto result = list.insert(make_pair(Name, std::shared_ptr<void>(Instance)));
		if (!result.second)
		{
			HelperDebug::Warning("system", "cannot add global (" + Name + ") because it already exists.");
			return Get<T>(Name);
		}
		return Instance;
	}
	template <typename T>
	T *Get(std::string Name)
	{
		auto i = list.find(Name);
		if(i == list.end())
		{
			HelperDebug::Crash("system", "cannot get global (" + Name + ") because it does not exists.");
			return nullptr;
		}
		return static_cast<T*>(i->second.get());
	}
	void Delete(std::string Name)
	{
		auto i = list.find(Name);
		if(i == list.end())
		{
			HelperDebug::Warning("system", "cannot delete global (" + Name + ") because it does not exists");
			return;
		}
		delete &i->second; // test this
		list.erase(i);
	}
private:
	std::unordered_map<std::string, std::shared_ptr<void>> list;
};


///////////////////////////////////////////////////////////
// System
///////////////////////////////////////////////////////////

//
// module
//

class Module
{
public:
	void Set(std::string Name, ManagerEvent *Event, ManagerEntity *Entity, ManagerGlobal *Global, v8::Persistent<v8::Context> Context, std::string *Message)
	{
		this->name    = Name;
		this->Event   = Event;
		this->Entity  = Entity;
		this->Global  = Global;
		this->Debug   = new HelperDebug(Name);
		this->File    = new HelperFile(Name);
		this->Opengl  = new HelperOpengl(Name);
		this->Script  = new HelperScript(Name, this, Context);
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
	HelperDebug   *Debug;
	HelperFile    *File;
	HelperOpengl  *Opengl;
	HelperScript  *Script;
private:
	std::string name;
	std::string *message;
};


//
// system
//

class System
{
public:
	System()
	{
		event   = new ManagerEvent();
		entity  = new ManagerEntity();
		global  = new ManagerGlobal();
		message = "";
	}

	void Init()
	{
		v8::Persistent<v8::Context> context = v8::Context::New();
		context->Enter();
		for (auto i : list)
		{
			std::get<1>(i)->Set(std::get<0>(i), event, entity, global, context, &message);
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
	std::string message;
};
