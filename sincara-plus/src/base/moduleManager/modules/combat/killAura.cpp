#include "killAura.h"
#include "menu/menu.h"
#include "util/logger/logger.h"
#include "util/math/math.h"
#include "util/minecraft/minecraft.h"
#include "moduleManager/commonData.h"
#include "configManager/configManager.h"
#include "configManager/settings.h"

#include <chrono>
#include <random>

bool KillAura::IsMouseMoving()
{
    static POINT lastMousePos;
    POINT currentMousePos;
    GetCursorPos(&currentMousePos);

    if (lastMousePos.x == currentMousePos.x && lastMousePos.y == currentMousePos.y)
    {
        return false;
    }

    lastMousePos = currentMousePos;
    return true;
}

// Change all SDK::Minecraft references to just SDK::Minecraft
// For example:
bool KillAura::IsValidTarget(CEntityPlayer& player)
{
    if (!CommonData::SanityCheck()) return false;
    if (Menu::Open) return false;
    if (SDK::Minecraft->IsInGuiState() && !settings::KA_allowInventory) return false;
    if (settings::KA_weaponOnly && !MinecraftUtils::IsWeapon(SDK::Minecraft->thePlayer->GetInventory().GetCurrentItem())) return false;
    if (settings::KA_mouseMoveCheck && !IsMouseMoving()) return false;
    if (settings::KA_visibilityCheck && !SDK::Minecraft->thePlayer->CanEntityBeSeen(player.GetInstance())) return false;
    if (settings::KA_invisibleCheck && player.IsInvisibleToPlayer(SDK::Minecraft->thePlayer->GetInstance())) return false;
    
    return true;
}

void KillAura::FindBestTarget(const std::vector<CommonData::PlayerData>& playerList, CommonData::PlayerData& bestTarget)
{
    float finalDist = FLT_MAX;
    float finalDiff = 370;
    float finalHealth = FLT_MAX;

    Vector3 pos = SDK::Minecraft->thePlayer->GetPos();
    Vector3 headPos = SDK::Minecraft->thePlayer->GetEyePos();
    Vector2 currentLookAngles = SDK::Minecraft->thePlayer->GetAngles();

    for (CommonData::PlayerData player : playerList)
    {
        if (!Java::Env->IsSameObject(SDK::Minecraft->thePlayer->GetInstance(), player.obj.GetInstance()) && 
            !(settings::KA_ignoreFriends && ConfigManager::IsFriend(player.name))) 
        {
            Vector3 playerPos = player.pos;
            float playerHeight = player.height;
            
            // Define multiple body points (head, chest, legs)
            Vector3 bodyPoints[] = {
                playerPos + Vector3(0, playerHeight - 0.1f, 0),  // Head
                playerPos + Vector3(0, playerHeight * 0.7f, 0), // Chest
                playerPos + Vector3(0, playerHeight * 0.3f, 0)  // Legs
            };

            for (Vector3 bodyPoint : bodyPoints)
            {
                Vector2 angles = Math::getAngles(headPos, bodyPoint);
                Vector2 difference = Math::vec_wrapAngleTo180(currentLookAngles.Invert() - angles.Invert());
                if (difference.x < 0) difference.x = -difference.x;
                if (difference.y < 0) difference.y = -difference.y;

                Vector3 diff = pos - playerPos;
                float dist = sqrt(pow(diff.x, 2.f) + pow(diff.y, 2.f) + pow(diff.z, 2.f));

                if ((abs(difference.x) <= settings::KA_fov) && dist <= settings::KA_range)
                {
                    float health = player.health;
                    switch(settings::KA_targetPriority)
                    {
                    case 1: // Health
                        if (finalHealth > health)
                        {
                            bestTarget = player;
                            finalHealth = health;
                            renderData = bodyPoint; // Store the hit point
                        }
                        break;
                    case 2: // Angle
                        if (finalDiff > difference.x)
                        {
                            bestTarget = player;
                            finalDiff = difference.x;
                            renderData = bodyPoint;
                        }
                        break;
                    default: // Distance
                        if (finalDist > dist) 
                        {
                            bestTarget = player;
                            finalDist = dist;
                            renderData = bodyPoint;
                        }
                    }
                }
            }
        }
    }
}

void KillAura::AttackTarget(CEntityPlayer& target)
{
    static bool shouldDrop = false;
    static bool shouldSpike = false;
    static float normalCps = 0.0f;
    static float lastKurtosisValue = 0.0f;
    static std::mt19937 gen(std::random_device{}()); // Keep only this

    long milli = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    if (milli - lastLeftClickTime < (1000 / nextLeftCps)) 
        return;

    // REMOVE these lines:
    // std::random_device rd;
    // std::mt19937 gen(rd());

    // Advanced randomization features
    if (settings::KA_advancedMode) {
        // Drop chance calculation
        if (settings::KA_dropChance > 0) {
            std::uniform_real_distribution<> dropDist(0.0, 100.0);
            shouldDrop = dropDist(gen) < settings::KA_dropChance;
        }

        // Spike chance calculation
        if (settings::KA_spikeChance > 0) {
            std::uniform_real_distribution<> spikeDist(0.0, 100.0);
            shouldSpike = spikeDist(gen) < settings::KA_spikeChance;
        }

        // Kurtosis-based CPS distribution
        if (settings::KA_kurtosis > 0) {
            float meanCps = (settings::KA_leftMinCps + settings::KA_leftMaxCps) / 2.0f;
            
            if (normalCps == 0.0f || std::abs(settings::KA_kurtosis - lastKurtosisValue) > 0.1f) {
                std::normal_distribution<float> normalDist(
                    meanCps, 
                    (settings::KA_leftMaxCps - settings::KA_leftMinCps) / 
                    (4.0f + settings::KA_kurtosis * 2.0f));
                normalCps = normalDist(gen);
                lastKurtosisValue = settings::KA_kurtosis;
            }

            std::normal_distribution<float> walkDist(0.0f, 0.5f);
            normalCps += walkDist(gen);
            normalCps = std::max<float>(settings::KA_leftMinCps, std::min<float>(settings::KA_leftMaxCps, normalCps));
            nextLeftCps = static_cast<int>(normalCps);
        }

        // Skip click if we got a drop
        if (shouldDrop) {
            lastLeftClickTime = milli;
            shouldDrop = false;
            return;
        }

        // Apply spike multiplier if we got a spike
        int currentCps = nextLeftCps; // Declare currentCps here
        if (shouldSpike) {
            currentCps = std::min<int>(static_cast<int>(nextLeftCps * settings::KA_spikeMultiplier), 25);
            shouldSpike = false;
        }

        // Burst pattern
        if (settings::KA_burstEnabled) {
            std::uniform_real_distribution<> burstChanceDist(0.0, 100.0);
            if (burstChanceDist(gen) < settings::KA_burstChance) {
                float meanCps = (settings::KA_leftMinCps + settings::KA_leftMaxCps) / 2.0f;
                currentCps = static_cast<int>(meanCps);
            }
        } // Added missing closing brace

        // Perform the actual click
        POINT pos_cursor;
        GetCursorPos(&pos_cursor);
        SendMessage(Menu::HandleWindow, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pos_cursor.x, pos_cursor.y));
        SendMessage(Menu::HandleWindow, WM_LBUTTONUP, 0, MAKELPARAM(pos_cursor.x, pos_cursor.y));
        lastLeftClickTime = milli;

        // Generate next CPS if not using kurtosis
        if (!settings::KA_advancedMode || settings::KA_kurtosis <= 0) {
            std::uniform_int_distribution<> distrib(settings::KA_leftMinCps, settings::KA_leftMaxCps);
            nextLeftCps = distrib(gen);
        }

        // Sword block logic remains the same
        if (settings::KA_swordBlock && MinecraftUtils::IsWeapon(SDK::Minecraft->thePlayer->GetInventory().GetCurrentItem())) {
            static bool blocked = false;
            if (target.GetHurtResistantTime() > 11 && !blocked) {
                SendMessage(Menu::HandleWindow, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(pos_cursor.x, pos_cursor.y));
                SendMessage(Menu::HandleWindow, WM_RBUTTONUP, 0, MAKELPARAM(pos_cursor.x, pos_cursor.y));
                blocked = true;
            }
            else if (target.GetHurtResistantTime() <= 11) {
                blocked = false;
            }
        }
    }
}

void KillAura::Update()
{
    if (!settings::KA_Enabled) {
        data = Vector3();
        return;
    }
    
    if (Menu::Open || !CommonData::SanityCheck() || SDK::Minecraft->IsInGuiState()) {
        data = Vector3();
        return;
    }

    if (settings::KA_mousePressCheck && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
        data = Vector3();
        return;
    }

    const auto& playerList = CommonData::nativePlayerList;
    if (playerList.empty()) {
        data = Vector3();
        return;
    }

    CommonData::PlayerData bestTarget;
    FindBestTarget(playerList, bestTarget);

    if (!bestTarget.obj.GetInstance() || !IsValidTarget(bestTarget.obj)) {
        data = Vector3();
        return;
    }

    Vector3 ePos = bestTarget.pos;
    Vector3 headPos = SDK::Minecraft->thePlayer->GetEyePos();
    Vector2 currentLookAngles = SDK::Minecraft->thePlayer->GetAngles();

    // Calculate angles to head and feet
    float eHeight = bestTarget.height - 0.1f;
    Vector3 eHeadPos = ePos + Vector3(0, eHeight, 0);
    
    Vector2 anglesFoot = Math::getAngles(headPos, ePos);
    Vector2 anglesHead = Math::getAngles(headPos, eHeadPos);

    // Calculate angle differences with improved smoothing
    Vector2 difference = Math::vec_wrapAngleTo180(currentLookAngles.Invert() - anglesHead.Invert());
    Vector2 differenceFoot = Math::vec_wrapAngleTo180(currentLookAngles.Invert() - anglesFoot.Invert());

    // Apply jitter if enabled
    if (settings::KA_jitterAmount > 0.0f) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> jitterDist(
            -settings::KA_jitterAmount, 
            settings::KA_jitterAmount);
        
        difference.x += jitterDist(gen);
        difference.y += jitterDist(gen) * 0.5f; // Less vertical jitter
    }

    // Apply exponential smoothing using the lock smooth setting
    float targetYaw = currentLookAngles.x + (difference.x * settings::KA_lockSmooth);

    // Handle vertical aiming with independent smoothing
    if (currentLookAngles.y > anglesFoot.y || currentLookAngles.y < anglesHead.y) {
        float targetPitchFoot = currentLookAngles.y + (differenceFoot.y * settings::KA_lockSmooth * 0.5f);
        float targetPitchHead = currentLookAngles.y + (difference.y * settings::KA_lockSmooth * 0.5f);
        
        // Choose closest correction point
        float targetPitch = (abs(currentLookAngles.y - anglesFoot.y) < 
                           abs(currentLookAngles.y - anglesHead.y)) ? 
                           targetPitchFoot : targetPitchHead;
        
        SDK::Minecraft->thePlayer->SetAngles(Vector2(targetYaw, targetPitch));
    } else {
        // Free vertical movement when between bounds
        SDK::Minecraft->thePlayer->SetAngles(Vector2(targetYaw, currentLookAngles.y));
    }

    AttackTarget(bestTarget.obj);
}

void KillAura::RenderMenu()
{
    Menu::ToggleWithKeybind(&settings::KA_Enabled, settings::KA_Key);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
    Menu::HorizontalSeparator("KA_Sep1");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

    Menu::Slider("Range", &settings::KA_range, 1.f, 6.f);
    Menu::Slider("FOV", &settings::KA_fov, 1.f, 360.f);
    Menu::Slider("Lock Smoothness", &settings::KA_lockSmooth, 0.01f, 1.0f);
    Menu::Slider("Jitter Amount", &settings::KA_jitterAmount, 0.0f, 2.0f); // Add this line
    
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
    Menu::HorizontalSeparator("KA_Sep2");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

    Menu::Slider("Left Min CPS", &settings::KA_leftMinCps, 1, settings::KA_leftMaxCps);
    Menu::Slider("Left Max CPS", &settings::KA_leftMaxCps, settings::KA_leftMinCps, 20);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
    Menu::HorizontalSeparator("KA_Sep3");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

    const char* targetPriorities[] = { "Distance", "Health", "Closest to Crosshair" };
    if (ImGui::BeginCombo("Target Priority", targetPriorities[settings::KA_targetPriority]))
    {
        for (int i = 0; i < IM_ARRAYSIZE(targetPriorities); i++)
        {
            bool isSelected = (settings::KA_targetPriority == i);
            if (ImGui::Selectable(targetPriorities[i], isSelected))
                settings::KA_targetPriority = i;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
    Menu::HorizontalSeparator("KA_Sep4");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

    Menu::Checkbox("Mouse Press Check", &settings::KA_mousePressCheck); // Add this line
    Menu::Checkbox("Weapon Only", &settings::KA_weaponOnly);
    Menu::Checkbox("Sword Block", &settings::KA_swordBlock);
    Menu::Checkbox("Allow in Inventory", &settings::KA_allowInventory);
    Menu::Checkbox("Require Mouse Move", &settings::KA_mouseMoveCheck);
    Menu::Checkbox("Visibility Check", &settings::KA_visibilityCheck);
    Menu::Checkbox("Ignore Invisible", &settings::KA_invisibleCheck);
    Menu::Checkbox("Ignore Friends", &settings::KA_ignoreFriends);
    Menu::Checkbox("Advanced Randomization Mode", &settings::KA_advancedMode);
    if (settings::KA_advancedMode) {
        Menu::Slider("Drop Chance", &settings::KA_dropChance, 0.f, 20.f);
        Menu::Slider("Spike Chance", &settings::KA_spikeChance, 0.f, 30.f);
        if (settings::KA_spikeChance > 0.0f) {
            Menu::Slider("Spike Multiplier", &settings::KA_spikeMultiplier, 1.f, 3.f);
        }
        Menu::Slider("Kurtosis", &settings::KA_kurtosis, 0.f, 5.f);
        Menu::Checkbox("Burst Pattern", &settings::KA_burstEnabled);
        if (settings::KA_burstEnabled) {
            Menu::Slider("Burst Chance", &settings::KA_burstChance, 5.f, 40.f);
        }
    }
} // <-- Add this closing brace to end KillAura::RenderMenu()
    