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
	struct Mesh { GLuint Positions, Normals, Texcoords, Elements; };
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

	// body
	typedef std::tuple<std::string, glm::vec3, bool> shape_key;
	struct shape_key_hash : public std::unary_function<shape_key, std::size_t>
	{
		std::size_t operator()(const shape_key& k) const
		{
			glm::vec3 vector = std::get<1>(k);
			int hash = 23;
			hash += 37 * std::hash<std::string>()(std::get<0>(k));
			hash += 37 * std::hash<float>()(vector.x);
			hash += 37 * std::hash<float>()(vector.y);
			hash += 37 * std::hash<float>()(vector.z);
			hash += 37 * std::hash<bool>()(std::get<2>(k));
			return hash;
		}
	};
	struct shape_key_equal : public std::binary_function<shape_key, shape_key, bool> { bool operator()(const shape_key& v0, const shape_key& v1) const { return v0 == v1; } };

	std::unordered_map<shape_key, btCollisionShape*, shape_key_hash> shapes;
	btRigidBody *CreateBody(std::string Path, glm::vec3 Scale = glm::vec3(1), float Mass = 0);
	btCollisionShape *GetShape(std::string Path, glm::vec3 Scale = glm::vec3(1), bool Static = true);
	void LoadShape(btCollisionShape *&Shape, std::string Path, glm::vec3 Scale = glm::vec3(1), bool Static = true);
	void LoadShapeCube(btCollisionShape *&Shape, glm::vec3 Scale = glm::vec3(1));
	void LoadShapePlane(btCollisionShape *&Shape);

public:
	// create
	unsigned int CreateModel(std::string Mesh, std::string Material, glm::vec3 Position = glm::vec3(0), glm::vec3 Rotation = glm::vec3(0), glm::vec3 Scale = glm::vec3(1), float Mass = 0);
	unsigned int CreateLight(glm::vec3 Position, float Radius, glm::vec3 Color = glm::vec3(1), float Intensity = 1.f, Light::Shape Type = Light::POINT);
	
	// callbacks
	void Callbacks();
	static v8::Handle<v8::Value> jsModel(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsLight(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsPosition(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsRotation(const v8::Arguments& args);
};
