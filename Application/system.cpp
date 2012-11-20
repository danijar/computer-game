#include "system.h"



// manager event

void ManagerEvent::Listen(string Name, FunctionVoid Callback)
{
	Listen(Name, 0, Callback);
}
void ManagerEvent::ListenData(string Name, FunctionData Callback)
{
	ListenData(Name, 0, Callback);
}
void ManagerEvent::Listen(string Name, int State, FunctionVoid Callback)
{
	Register(Name, State, Callback);
}
void ManagerEvent::ListenData(string Name, int State, FunctionData Callback)
{
	Register(Name, State, Callback);
}
void ManagerEvent::Register(string Name, int State, Function Callback)
{
	Key Key(Name, State);
	List[Key].push_back(Callback);
}
void ManagerEvent::Fire(string Name, void* Data)
{
	Fire(Name, 0, Data);
}
void ManagerEvent::Fire(string Name, int State, void* Data)
{
	Key Key(Name, State);
	Value Functions = List[Key];
	for (auto i = Functions.begin(); i != Functions.end(); i++) i->Call(Data);

	#ifdef _DEBUG
	cout << "Event " << Name; if(State) cout << "(" << State << ")"; cout << endl;
	#endif
}
void ManagerEvent::FireRange(string Name, int From, int To)
{
	for(int i = From; i <= To; i++) Fire(Name, i, NULL);
}



// manager storage

void ManagerStorage::Delete(string Name)
{
	auto i = List.find(Name);
	if(i != List.end()) List.erase(i);
}



// manager component

void Component::SetStorage(ManagerStorage* Storage)
{
	this->Storage = Storage;
}
void Component::SetEvent(ManagerEvent* Event)
{
	this->Event = Event;
}
void Component::SetMessage(string* Message)
{
	this->Message = Message;
}
void Component::Exit(string Message)
{
	*this->Message = Message;
	// break out of component update loop
}
ManagerComponent::ManagerComponent(string* Message)
{
	this->Message = Message;
}
void ManagerComponent::Add(string Name, Component* Component, ComponentType Type)
{
	List[Type][Name] = Component;
}
void ManagerComponent::Remove(string Name)
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
void ManagerComponent::Init(ManagerStorage* Storage, ManagerEvent* Event, string* Message)
{
	ComponentType Types[] = { Input, Calculation, Output };
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
void ManagerComponent::Update()
{
	ComponentType Types[] = { Input, Calculation, Output };
	for(int i = 0; i < sizeof(Types) / sizeof(Types[0]); i++) {
		auto List = this->List[Types[i]];
		for (auto i = List.begin(); i != List.end(); i++)
			if(*Message == "")
				i->second->Update();
	}
}



// system

System::System()
{
	Message = "";
	Components = new ManagerComponent(&Message);
	Events = new ManagerEvent();
	Storages = new ManagerStorage();
}
void System::Add(string Name, Component* Component, ComponentType Type)
{
	Components->Add(Name, Component, Type);
}
void System::Remove(string Name)
{
	Components->Remove(Name);
}
void System::Init()
{
	Components->Init(Storages, Events, &Message);
}
bool System::Update()
{
	Components->Update();
	if(Message == "") return true;
	else return false;
}
string System::UpdateWhile()
{
	while(Update());
	return Message;
}