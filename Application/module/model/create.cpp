#include "module.h"

#include <glew/glew.h>
#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>
using namespace std;
using namespace glm;

#include "form.h"
#include "model.h"
#include "light.h"


unsigned int ModuleModel::CreateModel(string Mesh, string Material, vec3 Position, vec3 Rotation, vec3 Scale, float Mass)
{
	unsigned int id = Entity->New();
	auto mdl = Entity->Add<Model>(id);
	auto frm = Entity->Add<Form>(id);

	ModuleModel::Mesh mesh = GetMesh(Mesh);
	mdl->Positions = mesh.Positions;
	mdl->Normals   = mesh.Normals;
	mdl->Texcoords = mesh.Texcoords;
	mdl->Elements  = mesh.Elements;

	ModuleModel::Material material = GetMaterial(Material);
	mdl->Diffuse     = GetTexture(material.Diffuse);
	//mdl->Normal   = textures.Get(material.Normal);
	//mdl->Specular = textures.Get(material.Specular);
	mdl->Material = Material;

	delete frm->Body;
	frm->Body = CreateBody(Mesh, Scale, Mass);
	frm->Position(Position);
	frm->Rotation(Rotation);
	frm->Scale(Scale);

	return id;
}

unsigned int ModuleModel::CreateLight(vec3 Position, float Radius, vec3 Color, float Intensity, Light::Shape Type)
{
	unsigned int id = Entity->New();
	auto frm = Entity->Add<Form>(id);
	auto lgh = Entity->Add<Light>(id);

	frm->Position(Position);
	lgh->Radius = Radius;
	lgh->Color = Color;
	lgh->Intensity = Intensity;
	lgh->Type = Type;

	return id;
}
