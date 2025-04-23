#pragma once
#include "util/math/geometry.h"
#include "moduleManager/moduleBase.h"
#include "moduleManager/commonData.h"
#include "configManager/settings.h"
#include "sdk/sdk.h"

class KillAura : public ModuleBase
{
public:
    Vector3 renderData;  // Add this line
    void Update() override;
    void RenderOverlay() override {}
    void RenderHud() override {}
    void RenderMenu() override;

    bool IsEnabled() override { return settings::KA_Enabled; }
    std::string GetName() override { return Name; }
    std::string GetCategory() override { return Category; }
    int GetKey() override { return settings::KA_Key; }

    void SetEnabled(bool enabled) override { settings::KA_Enabled = enabled; }
    void Toggle() override { settings::KA_Enabled = !settings::KA_Enabled; }

private:
    std::string Name = "Kill Aura";
    std::string Category = "Combat";

    bool IsMouseMoving();
    bool IsValidTarget(CEntityPlayer& player);
    void AttackTarget(CEntityPlayer& target);
    void FindBestTarget(const std::vector<CommonData::PlayerData>& playerList, CommonData::PlayerData& bestTarget);

    Vector3 data;
    bool pitchInfluenced = false;
    long lastLeftClickTime = 0;
    long lastRightClickTime = 0;
    int nextLeftCps = 10;
    int nextRightCps = 10;
};