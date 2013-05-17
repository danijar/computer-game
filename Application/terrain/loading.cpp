#include "module.h"

#include <string>
#include <atomic>
#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
using namespace std;
using namespace glm;

#include "terrain.h"
#include "form.h"
#include "model.h"



void ModuleTerrain::Loading()
{
	Debug->Pass("started loading thread");

	while(!cancle)
	{
		if(!loading) continue;

		loading_id = Load(loading_chunk);
		loading = false;
	}

	Debug->Pass("cancled loading thread");
}

unsigned int ModuleTerrain::Load(ivec3 Chunk)
{
	unsigned int id = GetChunk(Chunk);
	if(id)
	{
		Debug->Warning("chunk (" + to_string(Chunk.x) + ", " + to_string(Chunk.y) + ", " + to_string(Chunk.z) + ") was already loaded");
		return id;
	}

	Terrain *terrain;

	terrain->Chunk = Chunk;
	Generate(terrain);

	id = Entity->New();
	Mesh(id, terrain);
	
	Entity->Add<Terrain>(id, terrain);
	Entity->Add<Form>(id)->Position(vec3(Chunk * CHUNK));

	return id;
}

void ModuleTerrain::Update(ivec3 Chunk)
{
	unsigned int id = GetChunk(Chunk);
	auto trn = Entity->Get<Terrain>(id);

	Mesh(id, trn);

	Debug->Pass("updated a chunk");
}

void ModuleTerrain::Generate(Terrain *Data)
{
	for(int x = 0; x < CHUNK_X; ++x)
	{
		const float i = Data->Chunk.x + (float)x / CHUNK_X;
		for(int z = 0; z < CHUNK_Z; ++z)
		{
			const float j = Data->Chunk.z + (float)z / CHUNK_Z;

			double height_bias = 0.30;
			double height_base = 0.50 * (simplex(0.2f * vec2(i, j)) + 1) / 2;
			double height_fine = 0.20 * (simplex(1.5f * vec2(i, j)) + 1) / 2;
			int height = (int)((height_bias + height_base + height_fine) * CHUNK_Y);
			for(int y = 0; y < height && y < CHUNK.y; ++y) Data->Blocks[x][y][z] = rand() % 2 + 1;
		}
	}
}

void ModuleTerrain::Mesh(unsigned int Id, Terrain *Data)
{
	auto mdl = Entity->Check<Model>(Id) ? Entity->Get<Model>(Id) : Entity->Add<Model>(Id);

	// meshing
	vector<float> positions, normals, texcoords; vector<int> elements;

	int n = 0;
	for(int X = 0; X < CHUNK.x; ++X)
	for(int Y = 0; Y < CHUNK.y; ++Y)
	for(int Z = 0; Z < CHUNK.z; ++Z) {

		uint8_t type = Data->Blocks[X][Y][Z];
		if(!type) continue;

		for(int dim = 0; dim < 3; ++dim) { int dir = -1; do {
			ivec3 neigh = Shift(dim, ivec3(dir, 0, 0)) + ivec3(X, Y, Z);

			if(Inside(neigh, ivec3(0), CHUNK - 1))
				if(Data->Blocks[neigh.x][neigh.y][neigh.z])
					goto skip;

			{
				for(float i = 0; i <= 1; ++i)
				for(float j = 0; j <= 1; ++j)
				{
					vec3 vertex = vec3(X, Y, Z) + vec3(Shift(dim, ivec3((dir + 1) / 2, i, j)));
					positions.push_back(vertex.x); positions.push_back(vertex.y); positions.push_back(vertex.z);
				}

				vec3 normal = normalize(vec3(Shift(dim, ivec3(dir, 0, 0))));
				for(int i = 0; i < 4; ++i)
				{
					normals.push_back(normal.x); normals.push_back(normal.y); normals.push_back(normal.z);
				}

				vec2 position = vec2(type % TILES_U, type / TILES_U) * GRID;
				texcoords.push_back(position.x          + GAP); texcoords.push_back(position.y          + GAP);
				texcoords.push_back(position.x + GRID.x - GAP); texcoords.push_back(position.y          + GAP);
				texcoords.push_back(position.x          + GAP); texcoords.push_back(position.y + GRID.y - GAP);
				texcoords.push_back(position.x + GRID.x - GAP); texcoords.push_back(position.y + GRID.y - GAP);

				if(dir == -1) {
					elements.push_back(n+0); elements.push_back(n+1); elements.push_back(n+2);
					elements.push_back(n+1); elements.push_back(n+3); elements.push_back(n+2);
				} else {
					elements.push_back(n+0); elements.push_back(n+2); elements.push_back(n+1);
					elements.push_back(n+1); elements.push_back(n+2); elements.push_back(n+3);
				}
				n += 4;
			}

		skip: dir *= -1; } while(dir > 0); }
	}

	// buffering
	if(!mdl->Positions) glGenBuffers(1, &mdl->Positions);
	glBindBuffer(GL_ARRAY_BUFFER, mdl->Positions);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof GLfloat, &positions[0], GL_STATIC_DRAW);

	if(!mdl->Normals  ) glGenBuffers(1, &mdl->Normals  );
	glBindBuffer(GL_ARRAY_BUFFER, mdl->Normals);
	glBufferData(GL_ARRAY_BUFFER, normals.size()   * sizeof GLfloat, &normals[0],   GL_STATIC_DRAW);

	if(!mdl->Texcoords) glGenBuffers(1, &mdl->Texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, mdl->Texcoords);
	glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof GLfloat, &texcoords[0], GL_STATIC_DRAW);

	if(!mdl->Elements ) glGenBuffers(1, &mdl->Elements );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->Elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof GLuint, &elements[0], GL_STATIC_DRAW);

	mdl->Diffuse = texture;

	Debug->Pass("meshed a chunk with " + to_string(positions.size()) + " vertices");
}

bool ModuleTerrain::Inside(ivec3 Position, ivec3 Min, ivec3 Max)
{
	if(Position.x < Min.x || Position.y < Min.y || Position.z < Min.z) return false;
	if(Position.x > Max.x || Position.y > Max.y || Position.z > Max.z) return false;
	return true;
}

ivec3 ModuleTerrain::Shift(int Dimension, ivec3 Vector)
{
	if      (Dimension % 3 == 1) return ivec3(Vector.z, Vector.x, Vector.y);
	else if (Dimension % 3 == 2) return ivec3(Vector.y, Vector.z, Vector.x);
	else                         return Vector;
}
