#pragma once

#include "system.h"

#include <unordered_map>
#include <SFML/OpenGL.hpp>
#include <GLM/glm.hpp>
#include <V8/v8.h>

#include "light.h"


class ModuleModel : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();

	// mesh
	struct Mesh { GLuint Positions, Normals, Texcoords, Elements; };
	std::unordered_map<std::string, Mesh> meshes;
	Mesh GetMesh(std::string Path);
	void ReloadMeshes();
	void LoadMesh(Mesh &Mesh, std::string Path);
	void LoadQube(Mesh &Mesh);
	void LoadPlane(Mesh &Mesh);

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

	// creation
public:
	unsigned int Model(std::string Mesh, std::string Material, glm::vec3 Position = glm::vec3(0), glm::vec3 Rotation = glm::vec3(0), glm::vec3 Scale = glm::vec3(1), bool Static = true);
	unsigned int Light(glm::vec3 Position, float Radius, glm::vec3 Color = glm::vec3(1), float Intensity = 1.f, StorageLight::Shape Type = StorageLight::POINT);
	static v8::Handle<v8::Value> jsModel(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsLight(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsGetPosition(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsSetPosition(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsPrint(const v8::Arguments& args);
};
