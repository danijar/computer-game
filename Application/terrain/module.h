#pragma once

#include "system.h"

#include <vector>
#include <future>
#include <atomic>
#include <GLM/glm.hpp>
#include <GLEW/glew.h>
#include <SFML/Graphics.hpp>

#include "terrain.h"
#include "model.h"
#include "form.h"


#define TILES_U 4
#define TILES_V 4
#define GRID vec2(1.f / TILES_U, 1.f / TILES_V)
#define GAP float(0.0001)

class ModuleTerrain : public Module
{	
	// general
public:
	~ModuleTerrain();
private:
	void Init();
	void Update();
	void Listeners();
	void Callbacks();

	// texture
	GLuint texture;
	GLuint Texture();
	void TextureShrink(sf::Image &Texture);
	inline sf::Color TextureAverage(sf::Color A, sf::Color B, sf::Color C, sf::Color D);

	// loading
	std::atomic<bool> running, loading; std::mutex access; std::future<void> task;
	Terrain current; std::vector<GLfloat> positions, normals, texcoords; std::vector<GLuint> elements; std::vector<std::tuple<btVector3, btVector3, btVector3>> triangles; bool null;
	void Loading();
	void Generate(Terrain *Terrain);
	void Mesh(Terrain *Terrain);
	inline bool Inside(glm::ivec3 Position, glm::ivec3 Min, glm::ivec3 Max);
	inline glm::ivec3 Shift(int Dimension, glm::ivec3 Vector);
	void Buffer(unsigned int Id);

	// block
	uint8_t GetBlock(glm::ivec3 Block);
	void SetBlock(glm::ivec3 Block, uint8_t Type);
	unsigned int GetChunk(glm::ivec3 Chunk);
	glm::ivec3 PosChunk(glm::ivec3 Block);
	glm::ivec3 PosLocal(glm::ivec3 Block);

	// select
	GLuint marker, side;
	std::tuple<glm::ivec3, glm::ivec3, uint8_t> Selection();
	float Intbound(float s, float ds);
	int Signum(float x);
	GLuint Marker();
	uint8_t type;

public:
	// callbacks
	static v8::Handle<v8::Value> jsChunk(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsBlock(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsPlacetype(const v8::Arguments& args);
};
