#include "module.h"

#include <glew/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>

#include "type/form/type.h"
#include "type/model/type.h"
#include "type/light/type.h"

using namespace std;
using namespace glm;

uint64_t ModuleModel::CreateModel(string Mesh, string Material, vec3 Position, vec3 Rotation, vec3 Scale, float Mass)
{
	uint64_t id = Entity->New();
	auto mdl = Entity->Add<Model>(id);
	auto frm = Entity->Add<Form>(id);

	ModuleModel::Mesh mesh = GetMesh(Mesh);
	mdl->Positions = mesh.Positions;
	mdl->Normals   = mesh.Normals;
	mdl->Texcoords = mesh.Texcoords;
	mdl->Elements  = mesh.Elements;

	ModuleModel::Material material = GetMaterial(Material);
	mdl->Shininess = material.Shininess;
	mdl->Diffuse   = GetTexture(material.Diffuse);
	mdl->Normal    = GetTexture(material.Normal);
	mdl->Specular  = GetTexture(material.Specular);
	mdl->Material  = Material;

	mdl->Box = mesh.Box;

	delete frm->Body;
	frm->Body = CreateBody(Mesh, Scale, Mass);
	frm->Position(Position);
	frm->Rotation(Rotation);
	frm->Scale(Scale);

	return id;
}

uint64_t ModuleModel::CreateLight(vec3 Position, float Radius, vec3 Color, float Intensity, Light::Shape Type)
{
	uint64_t id = Entity->New();
	auto frm = Entity->Add<Form>(id);
	auto lgh = Entity->Add<Light>(id);

	frm->Position(Position);
	lgh->Radius = Radius;
	lgh->Color = Color;
	lgh->Intensity = Intensity;
	lgh->Type = Type;

	return id;
}
