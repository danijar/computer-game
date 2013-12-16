#include "module.h"

#include <dependency/glm/glm.hpp>
using namespace glm;

#include "type/terrain/type.h"


uint8_t ModuleTerrain::GetBlock(ivec3 Block)
{
	uint64_t id = GetChunk(PosChunk(Block));
	if(!id)
	{
		Log->Fail("the chunk isn't loaded");
		return 0;
	}
	
	ivec3 local = PosLocal(Block);
	return Entity->Get<Terrain>(id)->Blocks[local.x][local.y][local.z];
}

void ModuleTerrain::SetBlock(ivec3 Block, uint8_t Type)
{
	uint64_t id = GetChunk(PosChunk(Block));
	if(!id)
	{
		Log->Fail("the chunk isn't loaded");
		return;
	}

	auto trn = Entity->Get<Terrain>(id);

	ivec3 local = PosLocal(Block);
	trn->Blocks[local.x][local.y][local.z] = Type;
	trn->Changed = true;
}

uint64_t ModuleTerrain::GetChunk(ivec3 Chunk)
{
	for(auto i : Entity->Get<Terrain>())
		if(i.second->Key == Chunk)
			return i.first;
	return 0;
}

ivec3 ModuleTerrain::PosChunk(ivec3 Block)
{
	ivec3 chunk = Block / CHUNK_SIZE;
	ivec3 local = Block % CHUNK_SIZE;
	if (local.x < 0) { chunk.x--; local.x += CHUNK_SIZE.x; }
	if (local.y < 0) { chunk.y--; local.y += CHUNK_SIZE.y; }
	if (local.z < 0) { chunk.z--; local.z += CHUNK_SIZE.z; }
	return chunk;
}

ivec3 ModuleTerrain::PosLocal(ivec3 Block)
{
	ivec3 chunk = Block / CHUNK_SIZE;
	ivec3 local = Block % CHUNK_SIZE;
	if (local.x < 0) { chunk.x--; local.x += CHUNK_SIZE.x; }
	if (local.y < 0) { chunk.y--; local.y += CHUNK_SIZE.y; }
	if (local.z < 0) { chunk.z--; local.z += CHUNK_SIZE.z; }
	return local;
}
