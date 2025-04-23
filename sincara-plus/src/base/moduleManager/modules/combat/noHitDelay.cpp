#include "noHitDelay.h"
#include "sdk/sdk.h"
#include "java/java.h"
#include "moduleManager/commonData.h"

bool NoHitDelay::enabled = false;

NoHitDelay::NoHitDelay()
{
    // Constructor implementation
}

void NoHitDelay::Update()
{
    if (!enabled) return;
    if (!CommonData::SanityCheck()) return;

    // Reset left click counter
    Java::Env->SetIntField(SDK::Minecraft->GetInstance(), StrayCache::minecraft_leftClickCounter, 0);
}

void NoHitDelay::RenderOverlay()
{
    // Overlay rendering if needed
}

void NoHitDelay::RenderHud()
{
    // HUD rendering if needed
}

void NoHitDelay::RenderMenu()
{
    if (ImGui::BeginChild("NoHitDelay", ImVec2(425, 36), true))
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
        ImGui::Checkbox("No Hit Delay", &enabled);
    }
    ImGui::EndChild();
}
