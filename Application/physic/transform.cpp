#pragma once

#include "system.h"

#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "transform.h"


class ModuleTransform : public Module
{
	void Init()
	{

	}

	void Update()
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

	void Matrix(unsigned int id)
	{
		auto tsf = Entity->Get<StorageTransform>(id);

		mat4 Scale      = scale    (mat4(1), tsf->Scale);
		mat4 Translate  = translate(mat4(1), tsf->Position);
		mat4 Rotate     = rotate   (mat4(1), tsf->Rotation.x, vec3(1, 0 ,0))
						* rotate   (mat4(1), tsf->Rotation.y, vec3(0, 1, 0))
						* rotate   (mat4(1), tsf->Rotation.z, vec3(0, 0, 1));
		tsf->Matrix = Translate * Rotate * Scale;
	}
};
