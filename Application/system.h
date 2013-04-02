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
#include <V8/v8.h>


namespace system_h // instead use "detail" namespace for internal stuff and show other
{
	using namespace std; // get rid of that


	inline void Warning(string Message)
	{
		cout << "System Warning: " << Message << endl;
	}
	inline void Error(string Message)
	{
		cout << "System Error: " << Message << endl;
		cin.clear();
		cin.get();
		exit(EXIT_FAILURE);
	}


	///////////////////////////////////////////////////////////
	// Managers
	///////////////////////////////////////////////////////////

	//
	// manager event
	//

	class ManagerEvent
	{
		typedef unordered_map<string, unordered_map<int, vector<pair<void*, bool>>>> Events;
	public:
		void Listen(string Name, function<void()> Function)
		{
			Listen(Name, 0, Function);
		}
		void Listen(string Name, int State, function<void()> Function)
		{
			list[Name][State].push_back(make_pair(new function<void()>(Function), false));
		}
		template <typename T>
		void Listen(string Name, function<void(T)> Function)
		{
			Listen<T>(Name, 0, Function);
		}
		template <typename T>
		void Listen(string Name, int State, function<void(T)> Function)
		{
			list[Name][State].push_back(make_pair(new function<void(T)>(Function), true));
		}
		void Fire(string Name)
		{
			Fire(Name, 0);
		}
		void Fire(string Name, int State)
		{
			auto Functions = list[Name][State];

			for (auto i = Functions.begin(); i != Functions.end(); ++i)
			{
				if(i->second) continue;
				else          (*(function<void()>*)(i->first))();
			}
		}
		void FireRange(string Name, int From, int To)
		{
			for(int i = From; i <= To; ++i) Fire(Name, i);
		}
		template <typename T>
		void Fire(string Name, T Data)
		{
			Fire(Name, 0, Data);
		}
		template <typename T>
		void Fire(string Name, int State, T Data)
		{
			auto Functions = list[Name][State];

			for (auto i = Functions.begin(); i != Functions.end(); ++i)
			{
				if(i->second) (*(function<void(T)>*)i->first)(Data);
				else          (*(function<void()>*)i->first)();
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
		T* Add(unsigned int id)
		{
			if(id > index || id == 0) Error("Cannot add entity " + to_string(id) + " because it is not an entity id. Use 'int New()'.");
			auto key = type_index(typeid(T));

			if (list.find(key) == list.end())
			{
				auto value = new unordered_map<int, shared_ptr<void> >();
				list.insert(make_pair(key, *value));
			}
			T* t = new T();
			auto result = list[key].insert(make_pair(id, shared_ptr<void>(t)));
			if (!result.second)
			{
				Warning("Cannot add entity " + to_string(id) + " to " + string(key.name()) + " because it already exists.");
				return Get<T>(id);
			}
			return t;
		}
		template <typename T>
		unordered_map<int, T*> Get()
		{
			unordered_map<int, T*> output;
			auto key = type_index(typeid(T));
			if (Check(key))
			{
				for(auto i = list[key].begin(); i != list[key].end(); ++i)
				{
					output.insert(make_pair(i->first, static_cast<T*>(i->second.get())));
				}
			}
			return output;
		}
		template <typename T>
		T* Get(unsigned int id)
		{
			auto key = type_index(typeid(T));
			if (!Check(key, id))
			{
				Error("Cannot get entity because " + to_string(id) + " in " + string(key.name()) + " does not exist.");
				return nullptr;
			}
			else return static_cast<T*>(list[key][id].get());
		}
		void Delete(unsigned int id)
		{
			if(id > index || id == 0)
			{
				Warning("Cannot delete entity " + to_string(id) + " because it is not an entity id.");
				return;
			}
			for(auto i = list.begin(); i != list.end(); ++i)
			{
				if (Check(i->first, id))
				{
					auto j = i->second.find(id);
					j->second.reset();
					i->second.erase(j);
				}
			}
		}
		template <typename T>
		void Delete(unsigned int id)
		{
			auto key = type_index(typeid(T));
			if (!Check(key, id))
			{
				Error("Cannot delete entity because " + string(key.name()) + " in " + string(key.name()) + " does not exist.");
				return;
			}
			auto j = list[key].find(id);
			j->second.reset();
			list[key].erase(j);
		}
		template <typename T>
		bool Check(unsigned int id)
		{
			auto key = type_index(typeid(T));
			if (!Check(key)) return false;
			if (list[key].find(id) == list[key].end()) return false;
			return true;
		}
	private:
		unsigned int index;
		unordered_map<type_index, unordered_map<int, shared_ptr<void>>> list;
		bool Check(type_index key)
		{
			if (list.find(key) == list.end()) return false;
			return true;
		}
		bool Check(type_index key, int id)
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
		T* Add(string Name)
		{
			T* t = new T();
			auto result = list.insert(make_pair(Name, shared_ptr<void>(t)));
			if (!result.second)
			{
				Warning("Cannot add global " + Name + " because it already exists.");
				return Get<T>(Name);
			}
			return t;
		}
		template <typename T>
		T* Get(string Name)
		{
			auto i = list.find(Name);
			if(i == list.end())
			{
				Error("Cannot get global " + Name + " because it does not exists.");
				return nullptr;
			}
			return static_cast<T*>(i->second.get());
		}
		void Delete(string Name)
		{
			auto i = list.find(Name);
			if(i == list.end())
			{
				Warning("Cannot delete global " + Name + " because it does not exists.");
				return;
			}
			list.erase(i);
		}
	private:
		unordered_map<string, shared_ptr<void>> list;
	};


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
		void Info(std::string Message)
		{
			std::cout << name << " " << Message << std::endl;
		}
		void Inline(std::string Message)
		{
			std::cout << " " << Message;
		}
		void Pass(std::string Message)
		{
			std::cout << name << " " << Message << std::endl;
		}
		void Fail(std::string Message)
		{
			std::cout << name << " " << Message << std::endl;
		}
		void PassFail(std::string Message, bool Result, std::string Pass = "success", std::string Fail = "fail")
		{
			std::cout << name << " " << Message << " " << (Result ? Pass : Fail) << std::endl;
		}
		void Wait(std::string Message = "...")
		{
			std::cout << name << "" << Message << std::endl;
			std::cin.clear();
			std::cin.get();
		}
		void Crash(std::string Message)
		{
			std::cout << "crash" << " " << name << " " << Message << std::endl;
			std::cin.clear();
			std::cin.get();
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
			std::string path = name + "/" + Path;
			return "";
		}
		void Write(std::string Path, std::string Text)
		{
			std::string path = name + "/" + Path;
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
		HelperScript(std::string Name) : name(Name)
		{
			v8::Isolate* isolate = v8::Isolate::GetCurrent();
			v8::HandleScope scope(isolate);
			v8::Persistent<v8::Context> context = v8::Context::New();
			context->Enter();
		}
		void Bind(std::string Name, std::function<v8::Handle<v8::Value>(v8::Arguments const &)> Function)
		{
			v8::Isolate* isolate = v8::Isolate::GetCurrent();
			v8::HandleScope scope(isolate);
			v8::Local<v8::Context> context = v8::Context::GetCurrent();
			v8::InvocationCallback* function = Function.target<v8::InvocationCallback>();
			v8::Local<v8::Object> global = context->Global();
			global->Set(v8::String::New(Name.c_str()), v8::FunctionTemplate::New(*function)->GetFunction());
		}
		void Load(std::string Path, std::string Source = "")
		{
			if(scripts.find(Path) != scripts.end())
			{
				Warning("The script " + Path + " is already loaded.");
				return;
			}
			std::string path = name + "/" + Path;
			// actually load source from file here

			v8::Isolate* isolate = v8::Isolate::GetCurrent();
			v8::HandleScope scope(isolate);
			v8::Local<v8::Context> context = v8::Context::GetCurrent();
			v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::New(Source.c_str()));
			v8::Persistent<v8::Script> handle = v8::Persistent<v8::Script>::New(isolate, script);
			scripts.insert(std::make_pair(Path, handle));
		}
		v8::Persistent<v8::Value> Run(std::string Path)
		{
			if(scripts.find(Path) == scripts.end()) Load(Path);

			v8::Isolate* isolate = v8::Isolate::GetCurrent();
			v8::HandleScope scope(isolate);
			v8::Local<v8::Context> context = v8::Context::GetCurrent();
			v8::Local<v8::Value> result = scripts[Path]->Run();
			v8::Persistent<v8::Value> handle = v8::Persistent<v8::Value>::New(isolate, result);
			return handle;
		}
		~HelperScript()
		{
			v8::Isolate* isolate = v8::Isolate::GetCurrent();
			v8::HandleScope scope(isolate);
			v8::Local<v8::Context> context = v8::Context::GetCurrent();
			context->Exit();
			v8::V8::Dispose();
		}
	private:
		string name;
		std::unordered_map<std::string, v8::Persistent<v8::Script>> scripts;
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
		void Set(string Name, ManagerEvent* Event, ManagerEntity* Entity, ManagerGlobal* Global, string* Message)
		{
			this->name    = Name;
			this->Event   = Event;
			this->Entity  = Entity;
			this->Global  = Global;
			this->Debug   = new HelperDebug(Name);
			this->File    = new HelperFile(Name);
			this->Script  = new HelperScript(Name);
			this->message = Message;
		}
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual ~Module() {};
	protected:
		void Exit(string Message)
		{
			*this->message = Message;
		}
		string Name()
		{
			return name;
		}
		ManagerEvent  *Event;
		ManagerEntity *Entity;
		ManagerGlobal *Global;
		HelperDebug   *Debug;
		HelperFile    *File;
		HelperScript  *Script;
	private:
		string *message;
		string name;
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
			for (auto i = list.begin(); i != list.end(); ++i)
			for (auto j = i->second.begin(); j != i->second.end(); ++j)
			{
				j->second->Set(j->first, event, entity, global, &message);
				j->second->Init();
			}
			event->Fire("SystemInitialized");
		}

		void Add(string Name, Module* Module)
		{
			Add(0, Name, Module);
		}
		void Add(int Priority, string Name, Module* Module)
		{
			for(auto i : list[Priority])
			{
				if (i.first == Name)
				{
					Error("Cannot add module because there already is a module with this priority and name.");
					return;
				}
			}
			list[Priority].push_back(make_pair(Name, Module));
		}
		void Remove(string Name)
		{
			Remove(0, Name);
		}
		void Remove(int Priority, string Name)
		{
			for (auto i = list[Priority].begin(); i != list[Priority].end(); ++i)
			{
				if(i->first == Name)
				{
					list[Priority].erase(i);
					return;
				}
			}
			Warning("Cannot remove module because there is no module with this priority and name.");
		}
		bool Update()
		{
			for (auto i = list.begin(); i != list.end(); ++i)
			for (auto j = i->second.begin(); j != i->second.end(); ++j)
			{
				if(message != "") return false;
				j->second->Update();
			}
			event->Fire("SystemUpdated");
			return true;
		}
		string UpdateLoop()
		{
			while(Update());
			return message;
		}
		string GetMessage()
		{
			return message;
		}	
	private:
		private:
		map<int, vector<pair<string, Module*>>> list;
		ManagerEvent  *event;
		ManagerEntity *entity;
		ManagerGlobal *global;
		string message;
	};
}

using system_h::Module;
using system_h::System;
