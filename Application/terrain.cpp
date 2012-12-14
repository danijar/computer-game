#pragma once

#include "system.h"

#include "chunk.h"


class ComponentTerrain : public Component
{

	void Init()
	{
		auto chk = Storage->Add<StorageChunk>("chunk");
	}

	void Update()
	{

	}
};