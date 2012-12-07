#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional> 
#include <memory>

using namespace std;



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
		List[Name][State].push_back(make_pair(new function<void()>(Function), false));
	}
	template <typename T>
	void Listen(string Name, function<void(T)> Function)
	{
		Listen<T>(Name, 0, Function);
	}
	template <typename T>
	void Listen(string Name, int State, function<void(T)> Function)
	{
		List[Name][State].push_back(make_pair(new function<void(T)>(Function), true));
	}
	void Fire(string Name)
	{
		Fire(Name, 0);
	}
	void Fire(string Name, int State)
	{
		auto Functions = List[Name][State];

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
		auto Functions = List[Name][State];

		for (auto i = Functions.begin(); i != Functions.end(); ++i)
		{
			if(i->second) (*(function<void(T)>*)i->first)(Data);
			else          (*(function<void()>*)i->first)();
		}
	}
private:
	Events List;
};



// manager storage

struct Storage {
	virtual ~Storage() {}
};

class ManagerStorage
{
	typedef unordered_map<string, unique_ptr<Storage>> Storages;
public:
	template <typename T>
	T* Add(string Name)
	{
		T* t = new T();
		auto result = List.insert(make_pair(Name, unique_ptr<Storage>(t)));
		if (!result.second) throw runtime_error("Cannot add storage because " + Name + " already exists."); // doesn't work
		return t;
	}
	template <typename T>
	T* Get(string Name)
	{
		auto i = List.find(Name);
		return (i != List.end()) ? static_cast<T*>(i->second.get()) : nullptr;
	}
	void Delete(string Name)
	{
		auto i = List.find(Name);
		if(i != List.end()) List.erase(i);
	}
private:
	Storages List;
};



// manager component

enum Type{ Input, Calculation, Output };

class Component
{
public:
	virtual void Init(){}
	void SetStorage(ManagerStorage* Storage)
	{
		this->Storage = Storage;
	}
	void SetEvent(ManagerEvent* Event)
	{
		this->Event = Event;
	}
	void SetMessage(string* Message)
	{
		this->Message = Message;
	}
	virtual void Update() = 0;
protected:
	ManagerStorage* Storage;
	ManagerEvent* Event;
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
		this->Message = Message;
	}
	void Add(string Name, Component* Component, Type Type = Calculation)
	{
		List[Type][Name] = Component;
	}
	void Remove(string Name)
	{
		for (auto i = List.begin(); i != List.end(); i++)
		{
			auto List = i->second;
			if(List.find(Name) != List.end()){
				List.erase(Name);
				break;
			}
		}
	}
	void Init(ManagerStorage* Storage, ManagerEvent* Event, string* Message)
	{
		Type Types[] = { Input, Calculation, Output };
		for(int i = 0; i < sizeof(Types) / sizeof(Types[0]); i++) {
			auto List = this->List[Types[i]];
			for (auto i = List.begin(); i != List.end(); i++) {
				i->second->SetStorage(Storage);
				i->second->SetEvent(Event);
				i->second->SetMessage(Message);
				i->second->Init();
			}
		}
	}
	void Update()
	{
		Type Types[] = { Input, Calculation, Output };
		for(int i = 0; i < sizeof(Types) / sizeof(Types[0]); i++) {
			auto List = this->List[Types[i]];
			for (auto i = List.begin(); i != List.end(); i++)
				if(*Message == "")
					i->second->Update();
		}
	}
private:
	Components List;
	string* Message;
};



// system

class System
{
public:
	System()
	{
		Message = "";
		Components = new ManagerComponent(&Message);
		Events = new ManagerEvent();
		Storages = new ManagerStorage();	
	}
	void Add(string Name, Component* Component, Type Type = Calculation)
	{
		Components->Add(Name, Component, Type);
	}
	void Remove(string Name)
	{
		Components->Remove(Name);
	}
	void Init()
	{
		Components->Init(Storages, Events, &Message);
	}
	bool Update()
	{
		Components->Update();
		if(Message == "") return true;
		else return false;
	}
	string UpdateWhile()
	{
		while(Update());
		return Message;
	}
	string GetMessage()
	{
		return Message;
	}	
private:
	ManagerComponent* Components;
	ManagerEvent* Events;
	ManagerStorage* Storages;
	string Message;
};
