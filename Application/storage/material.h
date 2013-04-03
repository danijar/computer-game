#pragma once

#include <string>


struct StorageMaterial
{
	StorageMaterial() : Diffuse(0), Normal(0), Specular(0), Changed(true) {}
	std::string Path;
	unsigned int Diffuse, Normal, Specular;
	bool Changed;
};
