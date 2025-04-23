#include "Block.h"

#include "java/java.h"
#include "sdk/strayCache.h"

CBlock::CBlock()
{
	if (!StrayCache::initialized) StrayCache::Initialize();
	this->Class = StrayCache::block_class;
}

CBlock::CBlock(jobject instance) : CBlock()
{
	this->Instance = instance;
}

jclass CBlock::GetClass()
{
	return this->Class;
}

jobject CBlock::GetInstance()
{
	return this->Instance;
}

int CBlock::GetBlockId()
{
	return Java::Env->CallStaticIntMethod(this->GetClass(), StrayCache::block_getIdFromBlock, this->GetInstance());
}

// Add these helper methods:
bool CBlock::IsWood()
{
	int id = GetBlockId();
	// 17: Oak/Birch/Spruce/Jungle log, 162: Acacia/Dark Oak log, 5: Planks, 125: Double wood slab, 47: Bookshelf, 53: Oak stairs, 54: Chest, 72: Wooden pressure plate, 85: Fence, 99/100: Huge mushroom blocks, 158: Dropper, 134-136: Other wood stairs, 163-164: Acacia/Dark Oak stairs, 96: Trapdoor, 151: Daylight sensor, 158: Dropper, 146: Trapped chest, 158: Dropper, 198: End rod, 199: Chorus plant, 200: Chorus flower, 161: Leaves2, 18: Leaves
	return id == 17 || id == 162 || id == 5 || id == 125 || id == 47 || id == 53 || id == 54 ||
	       id == 72 || id == 85 || id == 99 || id == 100 || id == 134 || id == 135 || id == 136 ||
	       id == 163 || id == 164 || id == 96 || id == 151 || id == 146 || id == 158 || id == 198 ||
	       id == 199 || id == 200 || id == 161 || id == 18;
}

bool CBlock::IsWool()
{
	int id = GetBlockId();
	// 35: All wool colors (vanilla Minecraft uses metadata for color, but block ID is always 35)
	return id == 35;
}

bool CBlock::IsStone()
{
	int id = GetBlockId();
	// Example: 1 = stone, 4 = cobblestone, 98 = stone bricks, etc.
	return id == 1 || id == 4 || id == 98 || id == 109;
}

bool CBlock::IsOre()
{
	int id = GetBlockId();
	// Example: 14 = gold ore, 15 = iron ore, 16 = coal ore, 56 = diamond ore, etc.
	return id == 14 || id == 15 || id == 16 || id == 56 || id == 21 || id == 73 || id == 74 || id == 129;
}

bool CBlock::IsDirt()
{
	int id = GetBlockId();
	// Example: 2 = grass, 3 = dirt, 60 = farmland, etc.
	return id == 2 || id == 3 || id == 60;
}

bool CBlock::IsSand()
{
	int id = GetBlockId();
	// Example: 12 = sand, 24 = sandstone, etc.
	return id == 12 || id == 24;
}

bool CBlock::IsGravel()
{
	int id = GetBlockId();
	// Example: 13 = gravel
	return id == 13;
}

bool CBlock::IsClay()
{
	int id = GetBlockId();
	// Example: 82 = clay
	return id == 82;
}
