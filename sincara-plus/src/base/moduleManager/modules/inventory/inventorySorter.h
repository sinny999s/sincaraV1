#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>

#include "moduleManager/moduleBase.h"

struct PathNode
{
	int slotIndex;
	int mode;
	int button;
};

class InventorySorter : public ModuleBase
{
public:
	void Update() override;
	void RenderOverlay() override;
	void RenderHud() override {};

	void RenderMenu() override;

	bool IsEnabled() override { return settings::IS_Enabled; }
	std::string GetName() override { return Name; }
	std::string GetCategory() override { return Category; }
	int GetKey() override { return settings::IS_Key; }

	void SetEnabled(bool enabled) override { settings::IS_Enabled = enabled; }
	void Toggle() override { settings::IS_Enabled = !settings::IS_Enabled; }

private:
	bool isSorting;
	std::chrono::time_point<std::chrono::steady_clock> lastSortTime;
	std::chrono::time_point<std::chrono::steady_clock> activated;
	std::vector<PathNode> inventoryPath;
	int pathIndex = 0;

	bool isDroppingUselessItems;
	bool isDoingArmor;
	bool isDoingSwords;
	bool isCombiningStacks;

	void DropUselessItems();
	void DoArmor();
	void CombineStacks();
	void DoSwords();

	void GeneratePath();
	void ResetSort(bool enabled);
	void RenderInventoryEditor(bool& isOpen);
	void RenderCreateCategory(bool& isOpen, int categoryIndex);

private:
	std::string Name = "Inventory Sorter";
	std::string Category = "Inventory";
};