#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional> 
#include <memory>

using namespace std;



// manager event

typedef function<void()> FunctionVoid;
typedef function<void(void* Data)> FunctionData;

struct Function
{
public:
	Function(FunctionVoid Function) : Type(true) { PointerVoid = Function; }
	Function(FunctionData Function) : Type(false){ PointerData = Function; }
	bool IsVoid(){ return  Type; }
	bool IsData(){ return !Type; }
	void Call(void* Data = NULL){ if(Type) PointerVoid(); else PointerData(Data); };
private:
	bool Type;
	function<void()> PointerVoid;
	function<void(void*)> PointerData;
};

typedef pair<string, int> Key;
typedef vector<Function> Value;
struct Hash{ size_t operator()(Key x) const throw() { const char* s = x.first.c_str(); unsigned h = 0; while (*s) h = h * 101 + *s++; return h * 10 + x.second; } }; // improve hash some time
typedef unordered_map<Key, Value, Hash> ListEvent;

class ManagerEvent
{
public:
	void Listen(string Name, FunctionVoid Callback);
	void ListenData(string Name, FunctionData Callback);
	void Listen(string Name, int State, FunctionVoid Callback);
	void ListenData(string Name, int State, FunctionData Callback);
	void Fire(string Name, void* Data = NULL);
	void Fire(string Name, int State, void* Data = NULL);
	void FireRange(string Name, int From, int To);
private:
	void Register(string Name, int State, Function Function);
	ListEvent List;
};



// manager storage

struct Storage {
	virtual ~Storage() {}
};

typedef unordered_map<string, unique_ptr<Storage>> ListStorage;

class ManagerStorage
{
public:
	template <typename T>
	T* Add(string Name)
	{
		T* t = new T();
		auto result = List.insert(make_pair(Name, unique_ptr<Storage>(t)));
		//if (!result.second /* and if typename differs */) {
		//	throw std::runtime_error("Manager Storage Added Storage " + Name + " already exists.");
		//}
		return t;
	}
	template <typename T>
	T* Get(string Name)
	{
		auto i = List.find(Name);
		return (i != List.end()) ? static_cast<T*>(i->second.get()) : nullptr;
	}
	void Delete(string Name);
private:
	ListStorage List;
};



// manager tool

struct ManagerTool
{
	// fps counter
	// file access
	// time
	// timer
};



// manager component

class Component
{
public:
	virtual void Init(){}
	void SetStorage(ManagerStorage* Storage);
	void SetEvent(ManagerEvent* Event);
	void SetTool(ManagerTool* Tool);
	void SetMessage(string* Message);
	virtual void Update() = 0;
protected:
	ManagerStorage* Storage;
	ManagerEvent* Event;
	ManagerTool* Tool;
	void Exit(string Message);
private:
	string* Message;
};

enum ComponentType{ Input, Calculation, Output };
typedef unordered_map<ComponentType, unordered_map<string, Component*>> ListComponent;

class ManagerComponent
{
public:
	ManagerComponent(string* Message);
	void Add(string Name, Component* Component, ComponentType Type = Calculation);
	void Remove(string Name);
	void Init(ManagerStorage* Storage, ManagerEvent* Event, ManagerTool* Tool, string* Message);
	void Update();
private:
	ListComponent List;
	string* Message;
};



// system

class System
{
public:
	System();
	void Add(string Name, Component* Component, ComponentType Type = Calculation);
	void Remove(string Name);
	void Init();
	bool Update(); // in main.cpp while(System.Update);
	string UpdateWhile(); // return Message
	string GetMessage();

	ManagerStorage* Storages;
private:
	ManagerComponent* Components;
	ManagerEvent* Events;
	ManagerTool* Tools;
	string Message;
};
