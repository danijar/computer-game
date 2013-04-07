#include "module.h"

#include "transform.h"


void ModulePhysic::Init()
{

}

void ModulePhysic::Update()
{
	auto tfs = Entity->Get<StorageTransform>();
	for(auto i = tfs.begin(); i != tfs.end(); ++i)
	{
		if(i->second->Changed)
		{
			Matrix(i->first);
			i->second->Changed = false;
		}
		else if(!i->second->Static)
		{
			Matrix(i->first);
		}
	}
}
