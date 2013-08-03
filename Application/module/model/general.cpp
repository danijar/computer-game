#include "module.h"

#include <unordered_map>
#include <cstdlib>
#include <filesystem>
#include <GLEW/glew.h>
#include <GLM/glm.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;
using namespace std::tr2::sys;
using namespace sf;
using namespace glm;

#include "model.h"
#include "form.h"
#include "print.h"
#include "light.h"


void ModuleModel::Init()
{
	Opengl->Init();

	Listeners();
	Callbacks();

	Entity->Add<Print>(Entity->New())->Text = [=]{
		auto fms = Entity->Get<Model>();
		return "Forms     " + to_string(fms.size()      ) + '\n'
			 + "Meshes    " + to_string(meshes.size()   ) + '\n'
			 + "Materials " + to_string(materials.size()) + '\n'
			 + "Textures  " + to_string(textures.size() ) + '\n'
			 + "Shapes    " + to_string(shapes.size()   );
	};

	Script->Run("init.js");
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
		ReloadShapes();
	});
}

int ModuleModel::Hash(string Path)
{
	path filepath(Path);
	time_t timestamp = last_write_time(filepath);
	unsigned long long filesize = file_size(filepath);

	return hash<time_t>()(timestamp) + 37 * hash<unsigned long long>()(filesize);
}
