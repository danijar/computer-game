#pragma once

#include "system.h"

#include <unordered_map>
#include <SFML/OpenGL.hpp>
#include <GLM/glm.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
#include <V8/v8.h>

#include "light.h"


class ModuleModel : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();

	// mesh
	struct Mesh { GLuint Positions, Normals, Texcoords, Elements; }; // this is redundant to StorageMesh
	std::unordered_map<std::string, Mesh> meshes;
	Mesh GetMesh(std::string Path);
	void ReloadMeshes();
	void LoadMesh(Mesh &Mesh, std::string Path);
	void LoadMeshCube(Mesh &Mesh);
	void LoadMeshPlane(Mesh &Mesh);

	// material
	struct Material { std::string Name; std::string Diffuse, Normal, Specular; };
	std::unordered_map<std::string, Material> materials;
	Material GetMaterial(std::string Path);
	void ReloadMaterials();
	void LoadMaterial(Material &Material, std::string Path);

	// texture
	std::unordered_map<std::string, GLuint> textures;
	GLuint GetTexture(std::string Path);
	void ReloadTextures();
	void LoadTexture(GLuint &Texture, std::string Path);

	// bodies
	std::unordered_map<std::string, btCollisionShape*> staticshapes;
	std::unordered_map<std::string, btCollisionShape*> dynamicshapes;
	btRigidBody *CreateBody(std::string Path, float Mass = 0);
	btCollisionShape *GetShape(std::string Path, bool Static = true);
	void LoadShape(btCollisionShape *&Shape, std::string Path, bool Static = true);
	void LoadShapeCube(btCollisionShape *&Shape);
	void LoadShapePlane(btCollisionShape *&Shape);

	// create
public:
	unsigned int Model(std::string Mesh, std::string Material, glm::vec3 Position = glm::vec3(0), glm::vec3 Rotation = glm::vec3(0), glm::vec3 Scale = glm::vec3(1), float Mass = 0);
	unsigned int Light(glm::vec3 Position, float Radius, glm::vec3 Color = glm::vec3(1), float Intensity = 1.f, StorageLight::Shape Type = StorageLight::POINT, bool Static = true);
	// scripting
	static v8::Handle<v8::Value> jsModel(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsLight(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsGetPosition(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsSetPosition(const v8::Arguments& args);
};
