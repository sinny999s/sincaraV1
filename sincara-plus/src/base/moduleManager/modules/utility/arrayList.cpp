#include "arrayList.h"
#include "util/render/renderUtils.h"
#include "util/render/colorPresets.h"
#include "util/time/timeManager.h"

#include <imgui/imgui.h>
#include "menu/menu.h"
#include "moduleManager/commonData.h"
#include <moduleManager/moduleManager.h>

#include <algorithm>
#include <cmath>

#define M_PI 3.14159265358979323846

inline float Clamp(float value, float min, float max) {
    return value < min ? min : (value > max ? max : value);
}

ImVec4 LerpColors(const ImVec4& a, const ImVec4& b, float t) {
    return ImVec4(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t
    );
}

ImU32 ImLerp(ImU32 c1, ImU32 c2, float t) {
    ImVec4 col1 = ImGui::ColorConvertU32ToFloat4(c1);
    ImVec4 col2 = ImGui::ColorConvertU32ToFloat4(c2);
    return ImGui::ColorConvertFloat4ToU32(LerpColors(col1, col2, t));
}

ImVec4 GetColorForModule(int index, float rawTime) {
    if (settings::AL_colorMode == 0)
        return ImVec4(settings::AL_textColor[0], settings::AL_textColor[1], settings::AL_textColor[2], settings::AL_textColor[3]);

    float time = TimeManager::GetTime();
    float t = fmodf(time * settings::AL_rgbSpeed + (index * 0.1f), 2.0f * M_PI);
    float smoothT = (sinf(t) + 1.0f) * 0.5f;

    return ColorPresets::GetPreset(settings::AL_colorMode, t, smoothT, time);
}

void ArrayList::RenderHud() {
    if (!settings::AL_Enabled || !g_ModuleManager) return;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;

    float padding = settings::AL_backgroundPadding;
    float textSize = settings::AL_textSize;
    float leftMargin = 24.f; // Increase this value for more space from the left
    float startX = (settings::AL_renderPosition == 0 || settings::AL_renderPosition == 2) ? leftMargin : screenSize.x - 200.f - padding;
    float startY = (settings::AL_renderPosition == 0 || settings::AL_renderPosition == 1) ? 32.f + padding : screenSize.y - 32.f - padding;

    // Animation state for each module
    static std::map<std::string, float> animProgress;
    float animSpeed = 0.15f;

    // Gather enabled modules
    std::vector<std::string> enabledModules;
    for (const auto& modulePtr : g_ModuleManager->GetModules()) {
        if (modulePtr->IsEnabled())
            enabledModules.push_back(modulePtr->GetName());
    }

    // Sort by text width descending for a clean look
    std::sort(enabledModules.begin(), enabledModules.end(), [](const std::string& a, const std::string& b) {
        return Menu::Font->CalcTextSizeA(settings::AL_textSize, FLT_MAX, 0.0f, a.c_str()).x >
               Menu::Font->CalcTextSizeA(settings::AL_textSize, FLT_MAX, 0.0f, b.c_str()).x;
    });

    // Animate in/out progress for each module
    for (auto it = animProgress.begin(); it != animProgress.end(); ) {
        if (std::find(enabledModules.begin(), enabledModules.end(), it->first) == enabledModules.end()) {
            it->second -= animSpeed;
            if (it->second <= 0.f)
                it = animProgress.erase(it);
            else
                ++it;
        } else {
            it->second += animSpeed;
            if (it->second > 1.f) it->second = 1.f;
            ++it;
        }
    }
    for (const auto& name : enabledModules) {
        if (animProgress.find(name) == animProgress.end())
            animProgress[name] = 0.f;
    }

    float y = startY;
    int idx = 0;
    float time = TimeManager::GetTime();

    for (const auto& name : enabledModules) {
        float progress = Clamp(animProgress[name], 0.f, 1.f);
        if (progress <= 0.f) continue;

        float textWidth = Menu::Font->CalcTextSizeA(textSize, FLT_MAX, 0.0f, name.c_str()).x;
        float boxWidth = textWidth + 2 * padding + 24.f;
        float boxHeight = textSize + 2 * padding;

        float x = (settings::AL_renderPosition == 0 || settings::AL_renderPosition == 2) ? startX : screenSize.x - boxWidth - padding;

        // Slide-in animation from the left
        float slideOffset = (1.0f - progress) * 40.f;
        ImVec2 min = ImVec2(x - slideOffset, y);
        ImVec2 max = ImVec2(x + boxWidth - slideOffset, y + boxHeight);

        // Flipped animated color mode accent bar to the left of the text
        float fadeBarWidth = 18.f;
        // Use the same variables for both the gradient bar and background rectangle heights
        float barTop = min.y + 4.f;
        float barBottom = max.y - 4.f;
        ImVec2 fadeMin = ImVec2(min.x + 4.f, barTop);
        ImVec2 fadeMax = ImVec2(min.x + fadeBarWidth, barBottom);

        ImVec4 accentColor = GetColorForModule(idx, time);
        ImU32 fadeStart = ImGui::ColorConvertFloat4ToU32(ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.0f));
        ImU32 fadeEnd = ImGui::ColorConvertFloat4ToU32(ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.85f * progress));

        drawList->AddRectFilledMultiColor(
            fadeMin, fadeMax,
            fadeStart, fadeEnd, fadeEnd, fadeStart
        );

        // --- Background rectangle for text, no left-side rounding ---
        float gradientTextSpacing = 1.0f; // Smaller spacing
        ImVec2 textBgMin = ImVec2(min.x + fadeBarWidth, barTop);
        ImVec2 textBgMax = ImVec2(max.x, barBottom);
        ImU32 bgColor = ImGui::ColorConvertFloat4ToU32(ImVec4(
            settings::AL_backgroundColor[0],
            settings::AL_backgroundColor[1],
            settings::AL_backgroundColor[2],
            settings::AL_backgroundColor[3] * progress // Fade with animation
        ));
        float rounding = 6.0f;
        drawList->AddRectFilled(textBgMin, textBgMax, bgColor, rounding, ImDrawFlags_RoundCornersRight);
        // --- End background rectangle ---

        // Center the text vertically in the background
        float bgHeight = textBgMax.y - textBgMin.y;
        float textVerticalOffset = (bgHeight - textSize) / 2.0f;
        ImVec2 textPos = ImVec2(textBgMin.x + padding + gradientTextSpacing + 2.0f, textBgMin.y + textVerticalOffset);

        drawList->AddText(Menu::Font, textSize, ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, int(180 * progress)), name.c_str());

        ImVec4 textCol = GetColorForModule(idx, time);
        ImU32 textColor = ImGui::ColorConvertFloat4ToU32(ImVec4(textCol.x, textCol.y, textCol.z, 1.0f * progress));
        drawList->AddText(Menu::Font, textSize, textPos, textColor, name.c_str());

        y += boxHeight + 2.f; // Reduced spacing between modules
        idx++;
    }
}

void ArrayList::RenderMenu() {
    Menu::TextColored("This module is currently in development and may cause instability or crashes. Use with caution.", ImVec4(1.0f, 0.8f, 0.0f, 1.0f), FontSize::SIZE_16);

    Menu::ToggleWithKeybind(&settings::AL_Enabled, settings::AL_Key);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
    Menu::HorizontalSeparator("Sep1");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

    Menu::Dropdown("Position", settings::AL_renderPositionList, &settings::AL_renderPosition, 4);
    Menu::Slider("Text Size", &settings::AL_textSize, 1, 50);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
    Menu::HorizontalSeparator("Sep2");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

    Menu::Dropdown("Color Mode", settings::AL_colorModeList, &settings::AL_colorMode, 12);
    if (settings::AL_colorMode == 0) {
        Menu::ColorEdit("Color", settings::AL_textColor);
    }
    else {
        Menu::Slider("RGB Speed", &settings::AL_rgbSpeed, 0.1f, 5.0f, ImVec2(0, 0), "%.1f");
    }
    Menu::Slider("Background Padding", &settings::AL_backgroundPadding, 0, 20);
    Menu::ColorEdit("Background Color", settings::AL_backgroundColor);
}
