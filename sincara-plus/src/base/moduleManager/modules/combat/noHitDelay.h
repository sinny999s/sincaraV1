#pragma once
#include "moduleManager/moduleBase.h"
#include "menu/menu.h"

class NoHitDelay : public ModuleBase
{
public:
    NoHitDelay();
    
    void Update() override;
    void RenderOverlay() override;
    void RenderHud() override;
    void RenderMenu() override;
    
    bool IsEnabled() override { return enabled; }
    std::string GetName() override { return "No Hit Delay"; }
    std::string GetCategory() override { return "Combat"; }
    int GetKey() override { return 0; }
    
    void SetEnabled(bool state) override { enabled = state; }
    void Toggle() override { enabled = !enabled; }

    static bool enabled;
};