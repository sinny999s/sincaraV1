#include "base.h"
#include "sdk/sdk.h"

#include "../../ext/imgui/imgui.h"
#include "menu/menu.h"

#include "moduleManager/moduleManager.h"
#include "notificationManager/notificationManager.h"
#include <util/window/windowHelpers.h>
#include <moduleManager/commonData.h>
#include <util/math/math.h>
#include <ctime>

static void DrawLine(int x, int height, int padding)
{
	ImVec2 lineStart = ImVec2(x, settings::Hud_WatermarkPosition[1] + (padding * 1.5));
	ImVec2 lineEnd = ImVec2(x, settings::Hud_WatermarkPosition[1] + height - (padding * 1.5));
	ImGui::GetWindowDrawList()->AddLine(lineStart, lineEnd, IM_COL32(155, 155, 155, 255), 2);
}

static std::string floatToString(float value, int precision) {
	if (value == NAN || value == INFINITY || value == -INFINITY) return "0";

	std::ostringstream out;
	out << std::fixed << std::setprecision(precision) << value;
	return out.str();
}

static std::string yawToDirection(float yaw)
{
	float warpedYaw = Math::wrapAngleTo180(yaw);

	// 0 = South, 45 = South-West, 90 = West, 135 = North-West, -180 = North, -135 = North-East, -90 = East, -45 = South-East
	if (warpedYaw >= -22.5 && warpedYaw < 22.5) return "S";
	if (warpedYaw >= 22.5 && warpedYaw < 67.5) return "SW";
	if (warpedYaw >= 67.5 && warpedYaw < 112.5) return "W";
	if (warpedYaw >= 112.5 && warpedYaw < 157.5) return "NW";
	if (warpedYaw >= 157.5 || warpedYaw < -157.5) return "N";
	if (warpedYaw >= -157.5 && warpedYaw < -112.5) return "NE";
	if (warpedYaw >= -112.5 && warpedYaw < -67.5) return "E";
	if (warpedYaw >= -67.5 && warpedYaw < -22.5) return "SE";

	return "N/A";
}

void Base::RenderLoop() // Runs every frame
{
	if (!Base::Running || settings::Hud_DisableAllRendering) return;

	g_ModuleManager->RenderOverlay();

	if (Menu::OpenHudEditor)
	{
		// Render a black background to make the HUD editor more visible
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0, 0), windowSize, IM_COL32(0, 0, 0, 100));
	}

	if (settings::Hud_Watermark)
	{
		std::time_t now = std::time(nullptr);
		std::tm localTime{};
		localtime_s(&localTime, &now);
		char buffer[9];
		std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &localTime);

		const char* watermark = "Sincara+";
		std::string version = Base::version;
		std::string fps = std::to_string(CommonData::fps) + " FPS";
		std::string ping = std::to_string(CommonData::ping) + "ms";
		std::string coords = "X: " + floatToString(CommonData::playerPos.x, 1) + " Y: " + floatToString(CommonData::playerPos.y, 1) + " Z: " + floatToString(CommonData::playerPos.z, 1);
		std::string direction = yawToDirection(CommonData::playerYaw);
		std::string time = buffer;

		int margin = 10;
		int padding = 12;

		int watermarkSize = 24;
		int statsSize = 20;

		ImVec2 textSize = Menu::FontBold->CalcTextSizeA(watermarkSize, FLT_MAX, 0, watermark);
		ImVec2 versionSize = Menu::FontBold->CalcTextSizeA(statsSize, FLT_MAX, 0, version.c_str());
		ImVec2 fpsSize = Menu::FontBold->CalcTextSizeA(statsSize, FLT_MAX, 0, fps.c_str());
		ImVec2 pingSize = Menu::FontBold->CalcTextSizeA(statsSize, FLT_MAX, 0, ping.c_str());
		ImVec2 coordsSize = Menu::FontBold->CalcTextSizeA(statsSize, FLT_MAX, 0, coords.c_str());
		ImVec2 directionSize = Menu::FontBold->CalcTextSizeA(statsSize, FLT_MAX, 0, direction.c_str());
		ImVec2 timeSize = Menu::FontBold->CalcTextSizeA(statsSize, FLT_MAX, 0, "00:00:00");

		int totalTextWidth = textSize.x;
		if (settings::Hud_WatermarkVersion) totalTextWidth += statsSize + versionSize.x;
		if (settings::Hud_WatermarkFps)     totalTextWidth += statsSize + fpsSize.x;
		if (settings::Hud_WatermarkPing)    totalTextWidth += statsSize + pingSize.x;
		if (settings::Hud_WatermarkCoords)  totalTextWidth += statsSize + coordsSize.x;
		if (settings::Hud_WatermarkDirection) totalTextWidth += statsSize + directionSize.x;
		if (settings::Hud_WatermarkTime)    totalTextWidth += statsSize + timeSize.x;

		ImVec2 rectSize = ImVec2(totalTextWidth + padding * 2, textSize.y + padding * 2);

		ImGuiWindowFlags windowFlags = Menu::OpenHudEditor ? 0 : (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs);

		std::call_once(*Menu::setupWatermarkFlag, [&]() {
			ImGui::SetNextWindowPos(ImVec2(settings::Hud_WatermarkPosition[0], settings::Hud_WatermarkPosition[1]));
		});

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
		ImGui::SetNextWindowSize(rectSize);
		ImGui::Begin("Watermark", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | windowFlags);
		{
			settings::Hud_WatermarkPosition[0] = ImGui::GetWindowPos().x;
			settings::Hud_WatermarkPosition[1] = ImGui::GetWindowPos().y;

			ImVec2 winPos = ImGui::GetWindowPos();
			ImVec2 winSize = ImGui::GetWindowSize();
			ImDrawList* dl = ImGui::GetWindowDrawList();

			// Glassy black background with glow
			ImVec2 glowMin = ImVec2(winPos.x - 8, winPos.y - 8);
			ImVec2 glowMax = ImVec2(winPos.x + winSize.x + 8, winPos.y + winSize.y + 8);
			float glowAnim = (sinf(ImGui::GetTime() * 2.0f) + 1.0f) * 0.5f;
			ImU32 glowColor = IM_COL32(255, 255, 255, int(40 * glowAnim + 30));
			dl->AddRect(glowMin, glowMax, glowColor, 12.f, 0, 8.0f);

			dl->AddRectFilled(winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y), IM_COL32(20, 20, 20, 230), 10.f);
			dl->AddRect(winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y), IM_COL32(220, 220, 220, 180), 10.f, 0, 2.0f);

			// Draw watermark and stats
			ImVec2 textPos = ImVec2(winPos.x + padding, winPos.y + padding);
			dl->AddText(Menu::FontBold, watermarkSize, textPos, IM_COL32(255, 255, 255, 255), watermark);

			int currentX = textPos.x + textSize.x;
			auto drawStat = [&](const std::string& value, const ImVec2& size) {
				currentX += statsSize;
				// Vertical separator
				dl->AddLine(ImVec2(currentX - statsSize / 2, winPos.y + padding + 2), ImVec2(currentX - statsSize / 2, winPos.y + winSize.y - padding - 2), IM_COL32(220, 220, 220, 180), 1.5f);
				// Stat text
				dl->AddText(Menu::FontBold, statsSize, ImVec2(currentX, textPos.y + 2), IM_COL32(255, 255, 255, 255), value.c_str());
				currentX += size.x;
			};

			if (settings::Hud_WatermarkVersion)   drawStat(version, versionSize);
			if (settings::Hud_WatermarkFps)       drawStat(fps, fpsSize);
			if (settings::Hud_WatermarkPing)      drawStat(ping, pingSize);
			if (settings::Hud_WatermarkCoords)    drawStat(coords, coordsSize);
			if (settings::Hud_WatermarkDirection) drawStat(direction, directionSize);
			if (settings::Hud_WatermarkTime)      drawStat(time, timeSize);
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	g_ModuleManager->RenderHud();

	NotificationManager::Render();
}