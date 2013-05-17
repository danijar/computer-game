#include "module.h"

#include <GLM/glm.hpp>
using namespace glm;

#include "terrain.h"


uint8_t ModuleTerrain::GetBlock(ivec3 Block)
{
	unsigned int id = GetChunk(PosChunk(Block));
	if(!id)
	{
		Debug->Fail("the chunk isn't loaded");
		return 0;
	}
	
	ivec3 local = PosLocal(Block);
	return Entity->Get<Terrain>(id)->Blocks[local.x][local.y][local.z];
}

void ModuleTerrain::SetBlock(ivec3 Block, uint8_t Type)
{
	unsigned int id = GetChunk(PosChunk(Block));
	if(!id)
	{
		Debug->Fail("the chunk isn't loaded");
		return;
	}

	auto trn = Entity->Get<Terrain>(id);

	ivec3 local = PosLocal(Block);
	trn->Blocks[local.x][local.y][local.z] = Type;
	trn->Changed = true;
}

unsigned int ModuleTerrain::GetChunk(ivec3 Chunk)
{
	for(auto i : Entity->Get<Terrain>())
		if(i.second->Chunk == Chunk)
			return i.first;
	return 0;
}

ivec3 ModuleTerrain::PosChunk(ivec3 Block)
{
	ivec3 chunk = Block / ivec3(CHUNK_X, CHUNK_Y, CHUNK_Z);
	ivec3 local = Block % ivec3(CHUNK_X, CHUNK_Y, CHUNK_Z);
	if (local.x < 0) { chunk.x--; local.x += CHUNK_X; }
	if (local.y < 0) { chunk.y--; local.y += CHUNK_Y; }
	if (local.z < 0) { chunk.z--; local.z += CHUNK_Z; }
	return chunk;
}

ivec3 ModuleTerrain::PosLocal(ivec3 Block)
{
	ivec3 chunk = Block / ivec3(CHUNK_X, CHUNK_Y, CHUNK_Z);
	ivec3 local = Block % ivec3(CHUNK_X, CHUNK_Y, CHUNK_Z);
	if (local.x < 0) { chunk.x--; local.x += CHUNK_X; }
	if (local.y < 0) { chunk.y--; local.y += CHUNK_Y; }
	if (local.z < 0) { chunk.z--; local.z += CHUNK_Z; }
	return local;
}
