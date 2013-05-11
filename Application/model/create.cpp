#include "module.h"

#include <GLEW/glew.h>
#include <GLM/glm.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;
using namespace glm;

#include "transform.h"
#include "model.h"
#include "light.h"


unsigned int ModuleModel::Model(string Mesh, string Material, vec3 Position, vec3 Rotation, vec3 Scale, float Mass)
{
	unsigned int id = Entity->New();
	auto frm = Entity->Add<StorageModel>(id);
	auto tsf = Entity->Add<StorageTransform>(id);

	ModuleModel::Mesh mesh = GetMesh(Mesh);
	frm->Positions = mesh.Positions;
	frm->Normals   = mesh.Normals;
	frm->Texcoords = mesh.Texcoords;
	frm->Elements  = mesh.Elements;

	ModuleModel::Material material = GetMaterial(Material);
	frm->Diffuse     = GetTexture(material.Diffuse);
	//frm->Normal   = textures.Get(material.Normal);
	//frm->Specular = textures.Get(material.Specular);

	delete tsf->Body;
	tsf->Body = CreateBody(Mesh, Scale, Mass);
	tsf->Position(Position);
	tsf->Rotation(Rotation);
	tsf->Scale(Scale);

	return id;
}

unsigned int ModuleModel::Light(vec3 Position, float Radius, vec3 Color, float Intensity, StorageLight::Shape Type)
{
	unsigned int id = Entity->New();
	auto tsf = Entity->Add<StorageTransform>(id);
	auto lgh = Entity->Add<StorageLight>(id);

	tsf->Position(Position);
	lgh->Radius = Radius;
	lgh->Color = Color;
	lgh->Intensity = Intensity;
	lgh->Type = Type;

	return id;
}
