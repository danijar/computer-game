#include "module.h"

#include <cmath>
#include <GLM/glm.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace std;
using namespace glm;
using namespace sf;

#include "camera.h"


pair<ivec3, uint8_t> ModuleTerrain::Selection()
{
	auto cam = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"));
	Vector2u size = Global->Get<RenderWindow>("window")->getSize();

	vec3 origin = unProject(vec3(size.x/2, size.y/2, 0.f), cam->View, cam->Projection, vec4(0, 0, size.x, size.y));
	vec3 destination = unProject(vec3(size.x/2, size.y/2, 1.f), cam->View, cam->Projection, vec4(0, 0, size.x, size.y));

	float reach = 1000.f;

	int x = (int)floor(origin.x);
	int y = (int)floor(origin.y);
	int z = (int)floor(origin.z);

	float dx = destination.x - origin.x;
	float dy = destination.y - origin.y;
	float dz = destination.z - origin.z;

	int stepX = Signum(dx);
	int stepY = Signum(dy);
	int stepZ = Signum(dz);

	float tMaxX = Intbound(origin.x, dx);
	float tMaxY = Intbound(origin.y, dy);
	float tMaxZ = Intbound(origin.z, dz);

	float tDeltaX = stepX/dx;
	float tDeltaY = stepY/dy;
	float tDeltaZ = stepZ/dz;

	reach /= length(vec3(dx, dy, dz));

	uint8_t material = 0;
	while(!material)
	{
		if (tMaxX < tMaxY)
		{
			if (tMaxX < tMaxZ)
			{
				if (tMaxX > reach) break;
				x += stepX;
				tMaxX += tDeltaX;
			}
			else
			{
				z += stepZ;
				tMaxZ += tDeltaZ;
			}
		}
		else
		{
			if (tMaxY < tMaxZ)
			{
				if (tMaxY > reach) break;
				y += stepY;
				tMaxY += tDeltaY;
			}
			else
			{
				if (tMaxZ > reach) break;
				z += stepZ;
				tMaxZ += tDeltaZ;
			}
		}
		material = GetBlock(ivec3(x, y, z));
	}

	return make_pair(ivec3(x, y, z), material);
}

float ModuleTerrain::Intbound(float s, float ds)
{
	if (ds < 0) {
		return Intbound(-s, -ds);
	} else {
		s = fmod((fmod(s, 1) + 1), 1);
		return (1-s) / ds;
	}
}

int ModuleTerrain::Signum(float x)
{
	return x > 0 ? 1 : x < 0 ? -1 : 0;
}
