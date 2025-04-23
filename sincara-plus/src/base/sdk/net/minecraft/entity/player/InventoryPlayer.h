#pragma once

#include <vector>

#include "sdk/java/IClass.h"
#include "sdk/net/minecraft/item/ItemStack.h"

struct CInventoryPlayer : IClass
{
	CInventoryPlayer();
	CInventoryPlayer(jobject instance);

	jclass GetClass();
	jobject GetInstance();

	CItemStack GetCurrentItem();

	int GetCurrentItemIndex();
	void SetCurrentItemIndex(int index);

	std::vector<CItemStack> GetMainInventory();
	std::vector<CItemStack> GetArmorInventory();
};