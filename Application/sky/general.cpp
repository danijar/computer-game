#include "module.h"


void ModuleSky::Init()
{
	*Global->Add<Model>("sky") = CreateSky();
}

void ModuleSky::Update()
{
	
}
