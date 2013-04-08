#include "module.h"

#include <unordered_map>
#include <cstdlib>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/Window/Keyboard.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;
#include <V8/v8.h>

#include "model.h"
#include "transform.h"
#include "movement.h"
#include "animation.h"
#include "text.h"
#include "light.h"


void ModuleModel::Init()
{
	Opengl->Init();

	Listeners();

	Entity->Add<StorageText>(Entity->New())->Text = [=]{
		auto fms = Entity->Get<StorageModel>();
		return "Forms " + to_string(fms.size());
	};

	Script->Bind("model", jsModel);
	Script->Bind("light", jsLight);
	Script->Bind("getposition", jsGetPosition);
	Script->Bind("setposition", jsSetPosition);
	Script->Bind("print", jsPrint);

	Light(vec3(0.5f, 1.0f, 1.5f), 0.0f, vec3(0.75f, 0.74f, 0.67f), 0.2f, StorageLight::DIRECTIONAL);
	Script->Run("init.js");
	Script->Load("update.js"); // why doesn't it work without precompiling the script?
}

void ModuleModel::Update()
{
	Script->Run("update.js");
}

void ModuleModel::Listeners()
{
	Event->Listen("WindowFocusGained", [=]{
		ReloadMeshes();
		ReloadMaterials();
		ReloadTextures();
	});

	Event->Listen("InputBindCreate", [=]{
		// move this into a script
		bool many = Keyboard::isKeyPressed(Keyboard::LShift);
		for(int i = 0; i < (many ? 500 : 1); ++i)
		{
			unsigned int id = Model("qube.prim", "magic.mtl", vec3(0, 4, 0), vec3(0), vec3(1), false);
			Entity->Add<StorageMovement>(id);
			Entity->Add<StorageAnimation>(id);
			auto tsf = Entity->Get<StorageTransform>(id);
			tsf->Rotation = vec3(rand() % 360, rand() % 360, rand() % 360);
		}
	});
}

unsigned int ModuleModel::Model(string Mesh, string Material, vec3 Position, vec3 Rotation , vec3 Scale, bool Static)
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
	// frm->Normal   = textures.Get(material.Normal);
	// frm->Specular = textures.Get(material.Specular);

	tsf->Position  = Position;
	tsf->Rotation  = Rotation;
	tsf->Scale     = Scale;
	tsf->Static    = Static;

	return id;
}

unsigned int ModuleModel::Light(vec3 Position, float Radius, vec3 Color, float Intensity, StorageLight::Shape Type, bool Static)
{
	unsigned int id = Entity->New();
	auto tsf = Entity->Add<StorageTransform>(id);
	auto lgh = Entity->Add<StorageLight>(id);

	tsf->Position = Position;
	tsf->Static = Static;
	lgh->Radius = Radius;
	lgh->Color = Color;
	lgh->Intensity = Intensity;
	lgh->Type = Type;

	return id;
}

v8::Handle<v8::Value> ModuleModel::jsModel(const v8::Arguments& args)
{
	ModuleModel* module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	string mesh = *v8::String::Utf8Value(args[0]);
	string material = *v8::String::Utf8Value(args[1]);
	vec3 position(args[2]->NumberValue(), args[3]->NumberValue(), args[4]->NumberValue());
	vec3 rotation(args[5]->NumberValue(), args[6]->NumberValue(), args[7]->NumberValue());
	vec3 scale(args[8]->NumberValue());
	bool still = args[8]->BooleanValue();

	unsigned int id = module->Model(mesh, material, position, rotation, scale, still);
	return v8::Uint32::New(id);
}

v8::Handle<v8::Value> ModuleModel::jsLight(const v8::Arguments& args)
{
	ModuleModel* module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	vec3 position(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue());
	float radius = (float)args[3]->NumberValue();
	vec3 color(args[4]->NumberValue(), args[5]->NumberValue(), args[6]->NumberValue());
	float intensity  = (float)args[7]->NumberValue();
	bool statically = args[8]->BooleanValue();

	unsigned int id = module->Light(position, radius, color, intensity, StorageLight::POINT, statically);
	return v8::Uint32::New(id);
}

v8::Handle<v8::Value> ModuleModel::jsGetPosition(const v8::Arguments& args)
{
	ModuleModel* module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	unsigned int id = args[0]->Uint32Value();

	auto tsf = module->Entity->Get<StorageTransform>(id);
	vec3 position = tsf->Position;

	v8::Handle<v8::Array> result = v8::Array::New(3);
	result->Set(0, v8::Number::New(position.x));
	result->Set(1, v8::Number::New(position.y));
	result->Set(2, v8::Number::New(position.z));
	return result;
}

v8::Handle<v8::Value> ModuleModel::jsSetPosition(const v8::Arguments& args)
{
	ModuleModel* module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	unsigned int id = args[0]->Uint32Value();
	vec3 position(args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());

	auto tsf = module->Entity->Get<StorageTransform>(id);
	tsf->Position = position;
	return v8::Undefined();
}

v8::Handle<v8::Value> ModuleModel::jsPrint(const v8::Arguments& args)
{
	ModuleModel* module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	string message = *v8::String::Utf8Value(args[0]);

	module->Debug->Print(message);
	return v8::Undefined();
}
