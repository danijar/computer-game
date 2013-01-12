#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional> 
#include <memory>
#include <typeindex>

using namespace std;



// debug

#ifdef _DEBUG
#include <iostream>
#define Error(text)		do { cout << "System error: "   << text << endl; cin.clear(); cin.get(); exit(EXIT_FAILURE); } while(0)
#define Warning(text)	do { cout << "System warning: " << text << endl;											 } while(0)
#else
#define Error(text)		do {} while(0)
#define Warning(text)	do {} while(0)
#endif


// manager event

class ManagerEvent
{
	typedef unordered_map<string, unordered_map<int, vector<pair<void*, bool> > > > Events;
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



// manager entity

struct Storage {
	virtual ~Storage() {}
};

class ManagerEntity
{
	typedef unordered_map<type_index, unordered_map<int, shared_ptr<Storage> > > Entities;
public:
	ManagerEntity() : index(0) {}
	int New()
	{
		return ++index;
	}
	template <typename T>
	T* Add(unsigned int id)
	{
		if(id > index || id == 0) Error("Could not add " + to_string(id) + " because it is not an entity id. Use 'int New()'.");
		auto key = type_index(typeid(T));

		if (list.find(key) == list.end())
		{
			auto value = new unordered_map<int, shared_ptr<Storage> >();
			list.insert(make_pair(key, *value));
		}
		T* t = new T();
		auto result = list[key].insert(make_pair(id, shared_ptr<Storage>(t)));
		if (!result.second)
		{
			Warning("Could not add " + to_string(id) + " to " + string(key.name()) + " because it already exists.");
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
			Error("Could not get entity because " + to_string(id) + " in " + string(key.name()) + " does not exist.");
			return nullptr;
		}
		else return static_cast<T*>(list[key][id].get());
	}
	void Delete(unsigned int id)
	{
		if(id > index || id == 0)
		{
			Error("Could not delete " + to_string(id) + " because it is not an entity id.");
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
			Error("Could delete not entity because " + string(key.name()) + " in " + string(key.name()) + " does not exist.");
			return;
		}
		auto j = list[key].find(id);
		j->second.reset();
		list[key].erase(j);
	}
private:
	unsigned int index;
	Entities list;
	bool Check(type_index key)
	{
		if (list.find(key) == list.end()) return false;
		return true;
	}
	bool Check(type_index key, int entity)
	{
		if (!Check(key)) return false;
		if (list[key].find(entity) == list[key].end()) return false;
		return true;
	}
};




// manager global

class ManagerGlobal
{
	typedef unordered_map<string, unique_ptr<Storage>> Globals;
public:
	template <typename T>
	T* Add(string Name)
	{
		T* t = new T();
		auto result = list.insert(make_pair(Name, unique_ptr<Storage>(t)));
		if (!result.second)
		{
			Warning("Cannot add " + Name + " because it already exists.");
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
			Error("Cannot get " + Name + " because it does not exists.");
			return nullptr;
		}
		return static_cast<T*>(i->second.get());
	}
	void Delete(string Name)
	{
		auto i = list.find(Name);
		if(i == list.end())
		{
			Warning("Cannot delete " + Name + " because it does not exists.");
			return;
		}
		list.erase(i);
	}
private:
	Globals list;
};



// manager component

enum Type{ Input, Calculation, Output };

class Component
{
public:
	virtual void Init(){}
	void SetEvent(ManagerEvent* Event)
	{
		this->Event = Event;
	}
	void SetEntity(ManagerEntity* Entity)
	{
		this->Entity = Entity;
	}
	void SetGlobal(ManagerGlobal* Global)
	{
		this->Global = Global;
	}
	void SetMessage(string* Message)
	{
		this->Message = Message;
	}
	virtual void Update() = 0;
protected:
	ManagerEvent* Event;
	ManagerEntity* Entity;
	ManagerGlobal* Global;
	void Exit(string Message)
	{
		*this->Message = Message;
		// break out of component update loop
	}
private:
	string* Message;
};

class ManagerComponent
{
	typedef unordered_map<Type, unordered_map<string, Component*>> Components;
public:
	ManagerComponent(string* Message)
	{
		this->message = Message;
	}
	void Add(string Name, Component* Component, Type Type = Calculation)
	{
		list[Type][Name] = Component;
	}
	void Remove(string Name)
	{
		for (auto i = list.begin(); i != list.end(); i++)
		{
			auto list = i->second;
			if(list.find(Name) != list.end()){
				list.erase(Name);
				break;
			}
		}
	}
	void Init(ManagerEvent* Event, ManagerEntity* Entity, ManagerGlobal* Global, string* Message)
	{
		Type Types[] = { Input, Calculation, Output };
		for(int i = 0; i < sizeof(Types) / sizeof(Types[0]); i++) {
			auto list = this->list[Types[i]];
			for (auto i = list.begin(); i != list.end(); i++) {
				i->second->SetEvent(Event);
				i->second->SetEntity(Entity);
				i->second->SetGlobal(Global);
				i->second->SetMessage(Message);
				i->second->Init();
			}
		}
	}
	void Update()
	{
		Type Types[] = { Input, Calculation, Output };
		for(int i = 0; i < sizeof(Types) / sizeof(Types[0]); i++) {
			auto list = this->list[Types[i]];
			for (auto i = list.begin(); i != list.end(); i++)
				if(*message == "")
					i->second->Update();
		}
	}
private:
	Components list;
	string* message;
};



// system

class System
{
public:
	System()
	{
		message = "";
		component = new ManagerComponent(&message);
		event = new ManagerEvent();
		entity = new ManagerEntity();
		global = new ManagerGlobal();
	}
	void Add(string Name, Component* Component, Type Type = Calculation)
	{
		component->Add(Name, Component, Type);
	}
	void Remove(string Name)
	{
		component->Remove(Name);
	}
	void Init()
	{
		component->Init(event, entity, global, &message);
		event->Fire("SystemInitialized");
	}
	bool Update()
	{
		component->Update();
		event->Fire("SystemUpdated");
		if(message == "") return true;
		else return false;
	}
	string UpdateWhile()
	{
		while(Update());
		return message;
	}
	string GetMessage()
	{
		return message;
	}	
private:
	ManagerComponent* component;
	ManagerEvent* event;
	ManagerEntity* entity;
	ManagerGlobal* global;
	string message;
};
