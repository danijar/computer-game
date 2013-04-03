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


namespace system_h // instead use "detail" namespace for internal stuff and show other
{
	///////////////////////////////////////////////////////////
	// General
	///////////////////////////////////////////////////////////

	//
	// namespace, later on get rid of that
	//

	using namespace std;


	//
	// debug output, later on use HelperDebug
	//

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


	//
	// prototypes
	//

	class Module;


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
			std::cout << Name << " " << Message << std::endl;
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
			return Read(name, Path);
		}
		void Write(std::string Path, std::string Text)
		{
			Write(name, Path, Text);
		}

		static void Write(std::string Name, std::string Path, std::string Text)
		{
			std::string path = Name + "/" + Path;
			// write here
		}
		static std::string Read(std::string Name, std::string Path)
		{
			std::string path = Name + "/" + Path;
			return std::string((std::istreambuf_iterator<char>(std::ifstream(path))), std::istreambuf_iterator<char>());
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
			string message;

			for(count = 0; (result = glGetError()) != GL_NO_ERROR; ++count)
			{
					 if(result == GL_INVALID_ENUM)                      message = "GL_INVALID_ENUM";
				else if(result == GL_INVALID_VALUE)                     message = "GL_INVALID_VALUE";
				else if(result == GL_INVALID_OPERATION)                 message = "GL_INVALID_OPERATION";
				else if(result == GL_STACK_OVERFLOW)                    message = "GL_STACK_OVERFLOW";
				else if(result == GL_STACK_UNDERFLOW)                   message = "GL_STACK_UNDERFLOW";
				else if(result == GL_OUT_OF_MEMORY)                     message = "GL_OUT_OF_MEMORY";
				else if(result == GL_INVALID_FRAMEBUFFER_OPERATION_EXT) message = "GL_INVALID_FRAMEBUFFER_OPERATION_EXT";

				cout << message << endl;
			}
			if(count > 0) HelperDebug::Fail(Name, to_string(count) + " OpenGL error" + (count > 1 ? "s" : ""));

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
		HelperScript(string Name, Module* Module) : name(Name)
		{
			v8::Isolate* isolate = v8::Isolate::GetCurrent();
			v8::HandleScope scope(isolate);
			v8::Persistent<v8::Context> context = v8::Context::New();
			context->Enter();
			v8::Local<v8::External> handle = v8::External::New(reinterpret_cast<void*>(Module));
			module = v8::Persistent<v8::External>::New(isolate, handle);
		}
		~HelperScript()
		{
			v8::Isolate* isolate = v8::Isolate::GetCurrent();
			v8::HandleScope scope(isolate);
			v8::Local<v8::Context> context = v8::Context::GetCurrent();
			context->Exit();
		}

		void Bind(std::string Name, std::function<v8::Handle<v8::Value>(v8::Arguments const &)> Function)
		{
			v8::Isolate* isolate = v8::Isolate::GetCurrent();
			v8::HandleScope scope(isolate);
			v8::Local<v8::Context> context = v8::Context::GetCurrent();
			v8::InvocationCallback* function = Function.target<v8::InvocationCallback>();
			v8::Local<v8::Object> global = context->Global();
			global->Set(v8::String::New(Name.c_str()), v8::FunctionTemplate::New(*function, module)->GetFunction(), v8::ReadOnly);
		}
		void Load(std::string Path)
		{
			if(scripts.find(Path) != scripts.end())
			{
				Warning("The script " + Path + " is already loaded.");
				return;
			}

			std::string path = name + "/" + Path;
			std::string source = HelperFile::Read(name, Path);

			v8::Isolate* isolate = v8::Isolate::GetCurrent();
			v8::HandleScope scope(isolate);
			v8::Local<v8::Context> context = v8::Context::GetCurrent();
			v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::New(source.c_str()));
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
		std::unordered_map<std::string, v8::Persistent<v8::Script>> scripts;
		v8::Persistent<v8::External> module;
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
			this->Opengl  = new HelperOpengl(Name);
			this->Script  = new HelperScript(Name, this);
			this->message = Message;
		}
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual ~Module() {};
		string Name()
		{
			return name;
		}
	protected:
		void Exit(string Message)
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
