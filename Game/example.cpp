#include "example.h"

void Physics::Update()
{
	auto counter = Storage->Get<Integer>("counter");
	if(counter->Value > 5) Exit("Player is dead.");
	Event->Fire("counting", &counter->Value);
	counter->Value++;
}

void Physics::Init()
{
	Event->Listen("set", [=]{
		auto position = Storage->Add<Position>("position");
		position->X = 2;
		position->Y = 4;
		position->Z = 8;
	});

	Event->Listen("get", [=]{
		auto position = Storage->Get<Position>("position");
		cout << position->X << " " << position->Y << " " << position->Z << endl;
	});

	auto counter = Storage->Add<Integer>("counter");
	counter->Value = 0;

	Event->ListenData("counting", [=](void* x){
		cout << "Loop " << *(int*)x << endl;
	});

	Event->Fire("set");
	cout << "Coordinates: "; Event->Fire("get");
}