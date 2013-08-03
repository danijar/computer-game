#include "module.h"

#include <GLEW/glew.h>
#include <GLM/glm.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;
using namespace glm;

#include "form.h"
#include "model.h"
#include "light.h"


unsigned int ModuleModel::CreateModel(string Mesh, string Material, vec3 Position, vec3 Rotation, vec3 Scale, float Mass)
{
	unsigned int id = Entity->New();
	auto frm = Entity->Add<Model>(id);
	auto tsf = Entity->Add<Form>(id);

	ModuleModel::Mesh mesh = GetMesh(Mesh);
	frm->Positions = mesh.Positions;
	frm->Normals   = mesh.Normals;
	frm->Texcoords = mesh.Texcoords;
	frm->Elements  = mesh.Elements;

	ModuleModel::Material material = GetMaterial(Material);
	frm->Diffuse     = GetTexture(material.Diffuse);
	//frm->Normal   = textures.Get(material.Normal);
	//frm->Specular = textures.Get(material.Specular);
	frm->Material = Material;

	delete tsf->Body;
	tsf->Body = CreateBody(Mesh, Scale, Mass);
	tsf->Position(Position);
	tsf->Rotation(Rotation);
	tsf->Scale(Scale);

	return id;
}

unsigned int ModuleModel::CreateLight(vec3 Position, float Radius, vec3 Color, float Intensity, Light::Shape Type)
{
	unsigned int id = Entity->New();
	auto tsf = Entity->Add<Form>(id);
	auto lgh = Entity->Add<Light>(id);

	tsf->Position(Position);
	lgh->Radius = Radius;
	lgh->Color = Color;
	lgh->Intensity = Intensity;
	lgh->Type = Type;

	return id;
}
