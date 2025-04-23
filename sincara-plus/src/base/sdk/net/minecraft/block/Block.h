#pragma once

#include "sdk/java/IClass.h"

struct CBlock : IClass
{
	CBlock();
	CBlock(jobject instance);

	jclass GetClass();
	jobject GetInstance();

	int GetBlockId();

	// Add these helper methods:
	bool IsWood();
	bool IsStone();
	bool IsOre();
	bool IsDirt();
	bool IsSand();
	bool IsGravel();
	bool IsClay();
	bool IsWool(); // <-- Add this line
};