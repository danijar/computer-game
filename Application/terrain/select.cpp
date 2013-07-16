#include "module.h"

#include <cmath>
#include <GLM/glm.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace std;
using namespace glm;
using namespace sf;

#include "camera.h"
#include "settings.h"


tuple<ivec3, ivec3, uint8_t> ModuleTerrain::Selection()
{
	auto cam = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"));
	Vector2u size = Global->Get<RenderWindow>("window")->getSize();

	vec3 origin = unProject(vec3(size.x/2, size.y/2, 0.f), cam->View, cam->Projection, vec4(0, 0, size.x, size.y));
	vec3 destination = unProject(vec3(size.x/2, size.y/2, 1.f), cam->View, cam->Projection, vec4(0, 0, size.x, size.y));

	float reach = 1000.0f;

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
	ivec3 position, normal;
	while(!material)
	{
		if (tMaxX < tMaxY)
		{
			if (tMaxX < tMaxZ)
			{
				if (tMaxX > reach) break;
				x += stepX;
				tMaxX += tDeltaX;
				normal = vec3(-stepX, 0, 0);
			}
			else
			{
				if (tMaxZ > reach) break;
				z += stepZ;
				tMaxZ += tDeltaZ;
				normal = vec3(0, 0, -stepZ);
			}
		}
		else
		{
			if (tMaxY < tMaxZ)
			{
				if (tMaxY > reach) break;
				y += stepY;
				tMaxY += tDeltaY;
				normal = vec3(0, -stepY, 0);
			}
			else // repeated second case for easier conditionals
			{
				if (tMaxZ > reach) break;
				z += stepZ;
				tMaxZ += tDeltaZ;
				normal = vec3(0, 0, -stepZ);
			}
		}

		position = ivec3(x, y, z);

		// chunk isn't loaded yet
		if(!GetChunk(PosChunk(position)))
			break;
		
		// fetch block
		material = GetBlock(position);
	}

	return make_tuple(position, normal, material);
}

float ModuleTerrain::Intbound(float s, float ds)
{
	if (ds < 0) {
		return Intbound(-s, -ds);
	} else {
		s = fmod((fmod(s, 1.0f) + 1), 1.0f);
		return (1-s) / ds;
	}
}

int ModuleTerrain::Signum(float x)
{
	return x > 0 ? 1 : x < 0 ? -1 : 0;
}

bool ModuleTerrain::InReachDistance(vec3 Target)
{
	auto stg = Global->Get<Settings>("settings");
	auto frm = Entity->Get<Form>(*Global->Get<unsigned int>("camera"));

	float reach = (float)*stg->Get<int>("Placedistance");
	vec3 camera = frm->Position();
	return length(Target - camera) < reach;
}


#include <GLEW/glew.h>
#include <SFML/Graphics.hpp>

GLuint ModuleTerrain::Marker()
{
	unsigned int id = Entity->New();
	auto mdl = Entity->Add<Model>(id);
	auto frm = Entity->Add<Form>(id);

	const GLfloat Vertices[]  = {0,0,1.f,1.f,0,1.f,1.f,1.f,1.f,0,1.f,1.f,0,1.f,1.f,1.f,1.f,1.f,1.f,1.f,0,0,1.f,0,1.f,0,0,0,0,0,0,1.f,0,1.f,1.f,0,0,0,0,1.f,0,0,1.f,0,1.f,0,0,1.f,0,0,0,0,0,1.f,0,1.f,1.f,0,1.f,0,1.f,0,1.f,1.f,0,0,1.f,1.f,0,1.f,1.f,1.f};
	glGenBuffers(1, &mdl->Positions);
	glBindBuffer(GL_ARRAY_BUFFER, mdl->Positions);
	glBufferData(GL_ARRAY_BUFFER, 72 * sizeof(GLfloat), Vertices, GL_STATIC_DRAW);
	const GLfloat Normals[]   = {0,0,1,0,0,1,0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,1,0,0,1,0,0,1,0,0,1,0,0};
	glGenBuffers(1, &mdl->Normals);
	glBindBuffer(GL_ARRAY_BUFFER, mdl->Normals);
	glBufferData(GL_ARRAY_BUFFER, 72 * sizeof(GLfloat), Normals, GL_STATIC_DRAW);
	const GLfloat Texcoords[] = {0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.};
	glGenBuffers(1, &mdl->Texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, mdl->Texcoords);
	glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(GLfloat), Texcoords, GL_STATIC_DRAW);
	const GLuint  Elements[]  = {0,1,2,2,3,0,4,5,6,6,7,4,8,9,10,10,11,8,12,13,14,14,15,12,16,17,18,18,19,16,20,21,22,22,23,20};
	glGenBuffers(1, &mdl->Elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->Elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), Elements, GL_STATIC_DRAW);

	Image image;
	if(!image.loadFromFile(Name() + "/texture/marker.jpg"))
		Debug->Fail("marker texture cannot be loaded");

	glGenTextures(1, &mdl->Diffuse);
	glBindTexture(GL_TEXTURE_2D, mdl->Diffuse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	return id;
}
