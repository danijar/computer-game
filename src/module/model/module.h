#pragma once

#include "system/system.h"

#include <unordered_map>
#include <sfml/OpenGL.hpp>
#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>
#include <v8/v8.h>

#include "type/model/type.h"
#include "type/light/type.h"


class ModuleModel : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();
	void Callbacks();

	// mesh
	struct Mesh { GLuint Positions, Normals, Texcoords, Elements; Model::Bounds Box; };
	std::unordered_map<std::string, std::pair<Mesh, int>> meshes;
	Mesh GetMesh(std::string Path);
	void ReloadMeshes();
	void LoadMesh(Mesh &Mesh, std::string Path);
	void LoadMeshCube(Mesh &Mesh);
	void LoadMeshPlane(Mesh &Mesh, float Size = 10.0f);

	// material
	struct Material { std::string Name; float Shininess; std::string Diffuse, Normal, Specular; };
	std::unordered_map<std::string, std::pair<Material, int>> materials;
	Material GetMaterial(std::string Path);
	void ReloadMaterials();
	void LoadMaterial(Material &Material, std::string Path);

	// texture
	std::unordered_map<std::string, std::pair<GLuint, int>> textures;
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

	std::unordered_map<shape_key, std::pair<btCollisionShape*, int>, shape_key_hash> shapes;
	btRigidBody *CreateBody(std::string Path, glm::vec3 Scale = glm::vec3(1), float Mass = 0);
	btCollisionShape *GetShape(std::string Path, glm::vec3 Scale = glm::vec3(1), bool Static = true);
	void ReloadShapes();
	void LoadShape(btCollisionShape *&Shape, std::string Path, glm::vec3 Scale = glm::vec3(1), bool Static = true);
	void LoadShapeCube(btCollisionShape *&Shape, glm::vec3 Scale = glm::vec3(1));
	void LoadShapePlane(btCollisionShape *&Shape);

public:
	// create
	uint64_t CreateModel(std::string Mesh, std::string Material, glm::vec3 Position = glm::vec3(0), glm::vec3 Rotation = glm::vec3(0), glm::vec3 Scale = glm::vec3(1), float Mass = 0);
	uint64_t CreateLight(glm::vec3 Position, float Radius, glm::vec3 Color = glm::vec3(1), float Intensity = 1.0f, Light::Shape Type = Light::POINT);

	// callbacks
	static v8::Handle<v8::Value> jsModel    (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsLight    (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsPosition (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsRotation (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsScale    (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsDirection(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsRadius   (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsIntensity(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsColor    (const v8::Arguments& args);
};
