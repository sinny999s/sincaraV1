#pragma once

#include "sdk/net/minecraft/entity/EntityLivingBase.h"
#include "InventoryPlayer.h"

struct CEntityPlayer : CEntityLivingBase
{
	CEntityPlayer();
	CEntityPlayer(jobject instance);

	jclass GetClass();
	jobject GetInstance();

	CInventoryPlayer GetInventory();
	int GetPing();
    float GetSwingProgress(); // Add this declaration
};

