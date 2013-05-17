#include "module.h"

#include <string>
#include <atomic>
#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
#include <SFML/OpenGL.hpp>
using namespace std;
using namespace glm;
using namespace sf;

#include "terrain.h"
#include "form.h"
#include "model.h"



void ModuleTerrain::Loading()
{
	Debug->Pass("started loading thread");

	Context context;

	while(running)
		if(loading)
		{
			// just remesh updated chunk
			if(terrain->Changed)
			{
				Mesh(model, terrain);
			}
			// load or generate new chunk
			else
			{
				Generate(terrain);
				Mesh(model, terrain);
			}

			loading = false;
		}

	Debug->Pass("canceled loading thread");
}

void ModuleTerrain::Generate(Terrain *Terrain)
{
	for(int x = 0; x < CHUNK_X; ++x)
	{
		const float i = Terrain->Chunk.x + (float)x / CHUNK_X;
		for(int z = 0; z < CHUNK_Z; ++z)
		{
			const float j = Terrain->Chunk.z + (float)z / CHUNK_Z;

			double height_bias = 0.30;
			double height_base = 0.50 * (simplex(0.2f * vec2(i, j)) + 1) / 2;
			double height_fine = 0.20 * (simplex(1.5f * vec2(i, j)) + 1) / 2;
			int height = (int)((height_bias + height_base + height_fine) * CHUNK_Y);
			for(int y = 0; y < height && y < CHUNK.y; ++y) Terrain->Blocks[x][y][z] = rand() % 2 + 1;
		}
	}
}

void ModuleTerrain::Mesh(Model *Model, Terrain *Terrain)
{
	vector<float> positions, normals, texcoords; vector<int> elements;

	int n = 0;
	for(int X = 0; X < CHUNK.x; ++X)
	for(int Y = 0; Y < CHUNK.y; ++Y)
	for(int Z = 0; Z < CHUNK.z; ++Z) {

		uint8_t type = Terrain->Blocks[X][Y][Z];
		if(!type) continue;

		for(int dim = 0; dim < 3; ++dim) { int dir = -1; do {
			ivec3 neigh = Shift(dim, ivec3(dir, 0, 0)) + ivec3(X, Y, Z);

			if(Inside(neigh, ivec3(0), CHUNK - 1))
				if(Terrain->Blocks[neigh.x][neigh.y][neigh.z])
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

	if(!Model->Positions) glGenBuffers(1, &Model->Positions);
	glBindBuffer(GL_ARRAY_BUFFER, Model->Positions);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof GLfloat, &positions[0], GL_STATIC_DRAW);

	if(!Model->Normals  ) glGenBuffers(1, &Model->Normals  );
	glBindBuffer(GL_ARRAY_BUFFER, Model->Normals);
	glBufferData(GL_ARRAY_BUFFER, normals.size()   * sizeof GLfloat, &normals[0],   GL_STATIC_DRAW);

	if(!Model->Texcoords) glGenBuffers(1, &Model->Texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, Model->Texcoords);
	glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof GLfloat, &texcoords[0], GL_STATIC_DRAW);

	if(!Model->Elements ) glGenBuffers(1, &Model->Elements );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Model->Elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof GLuint, &elements[0], GL_STATIC_DRAW);

	Model->Diffuse = texture;

	Debug->Pass("meshed a chunk with " + to_string(positions.size()) + " vertices and " + to_string(elements.size()) + " elements.");
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
