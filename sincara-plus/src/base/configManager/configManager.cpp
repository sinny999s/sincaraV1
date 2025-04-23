#include "configManager.h"
#include <util/logger/logger.h>
#include <java/java.h>

bool ConfigManager::LoadConfig(const char* name)
{
	std::ifstream file(ConfigManager::GetConfigPath() + name + FILE_SUFFIX);

	if (!file.is_open())
		return false;

	json j;
	file >> j;
	file.close();

	if (!JsonToSettings(j))
		return false;

	return true;
}

int ConfigManager::SaveConfig(const char* name)
{
	json j;

	if (!SettingsToJson(j))
		return -1;

	std::ofstream file(ConfigManager::GetConfigPath() + name + FILE_SUFFIX);

	if (!file.is_open())
		return -1;

	file << j.dump();
	file.close();

	// Return the index of the saved config
	std::vector<std::string> configs = ConfigManager::GetConfigList();
	for (size_t i = 0; i < configs.size(); ++i)
	{
		if (configs[i] == name)
			return i;
	}

	return -1;
}

bool ConfigManager::RemoveConfig(const char* name)
{
	return std::filesystem::remove(ConfigManager::GetConfigPath() + name + FILE_SUFFIX);
}

std::vector<std::string> ConfigManager::GetConfigList()
{
	std::vector<std::string> configs;

	// check if the directory exists, if not create it
	if (!std::filesystem::exists(ConfigManager::GetConfigPath()))
		std::filesystem::create_directory(ConfigManager::GetConfigPath());

	for (const auto& entry : std::filesystem::directory_iterator(ConfigManager::GetConfigPath()))
	{
		if (entry.path().extension() == FILE_SUFFIX/* || entry.path().extension() == ".json"*/)
			configs.push_back(entry.path().filename().replace_extension("").string());
	}

	return configs;
}

bool ConfigManager::SettingsToJson(json& j)
{
	// Menu
	j["Menu_GUIMovement"] = settings::Menu_GUIMovement;
	j["Menu_ShowHiddenCategories"] = settings::Menu_ShowHiddenCategories;

	// Hud
	j["Hud_DisableAllRendering"] = settings::Hud_DisableAllRendering;
	j["Hud_Watermark"] = settings::Hud_Watermark;
	j["Hud_WatermarkPosition"] = { settings::Hud_WatermarkPosition[0], settings::Hud_WatermarkPosition[1] };
	j["Hud_WatermarkVersion"] = settings::Hud_WatermarkVersion;
	j["Hud_WatermarkFps"] = settings::Hud_WatermarkFps;
	j["Hud_WatermarkPing"] = settings::Hud_WatermarkPing;
	j["Hud_WatermarkCoords"] = settings::Hud_WatermarkCoords;
	j["Hud_WatermarkDirection"] = settings::Hud_WatermarkDirection;
	j["Hud_WatermarkTime"] = settings::Hud_WatermarkTime;
	j["Hud_ShowKeybinds"] = settings::Hud_ShowKeybinds;
	j["Hud_KeybindsPosition"] = { settings::Hud_KeybindsPosition[0], settings::Hud_KeybindsPosition[1] };

	// ESP
	j["ESP_Enabled"] = settings::ESP_Enabled;
	j["ESP_Key"] = settings::ESP_Key;
	j["ESP_FadeDistance"] = settings::ESP_FadeDistance;
	j["ESP_HealthBar"] = settings::ESP_HealthBar;
	j["ESP_BoxType"] = settings::ESP_BoxType;
	j["ESP_3DBoxColor"] = settings::ESP_3DBoxColor;
	j["ESP_3DBoxThickness"] = settings::ESP_3DBoxThickness;
	j["ESP_Box"] = settings::ESP_Box;
	j["ESP_BoxColor"] = settings::ESP_BoxColor;
	j["ESP_FilledBox"] = settings::ESP_FilledBox;
	j["ESP_FilledBoxColor"] = settings::ESP_FilledBoxColor;
	j["ESP_SecondFilledBoxColor"] = settings::ESP_SecondFilledBoxColor;
	j["ESP_Outline"] = settings::ESP_Outline;
	j["ESP_OutlineColor"] = settings::ESP_OutlineColor;
	j["ESP_HighlightFriends"] = settings::ESP_HighlightFriends;
	j["ESP_Friend3DBoxColor"] = settings::ESP_Friend3DBoxColor;
	j["ESP_FriendBoxColor"] = settings::ESP_FriendBoxColor;
	j["ESP_FriendFilledBoxColor"] = settings::ESP_FriendFilledBoxColor;
	j["ESP_FriendSecondFilledBoxColor"] = settings::ESP_FriendSecondFilledBoxColor;
	j["ESP_FriendOutlineColor"] = settings::ESP_FriendOutlineColor;

	// No Hit Delay
	j["NHD_Enabled"] = settings::NHD_Enabled;
	
	// Kill Aura
	j["KA_Enabled"] = settings::KA_Enabled;
	j["KA_Key"] = settings::KA_Key;
	j["KA_range"] = settings::KA_range;
	j["KA_fov"] = settings::KA_fov;
	j["KA_advancedMode"] = settings::KA_advancedMode;
	j["KA_dropChance"] = settings::KA_dropChance;
	j["KA_spikeChance"] = settings::KA_spikeChance;
	j["KA_spikeMultiplier"] = settings::KA_spikeMultiplier;
	j["KA_kurtosis"] = settings::KA_kurtosis;
	j["KA_burstEnabled"] = settings::KA_burstEnabled;
	j["KA_burstChance"] = settings::KA_burstChance;
	j["KA_lockSmooth"] = settings::KA_lockSmooth;
	j["KA_jitterAmount"] = settings::KA_jitterAmount;
	j["KA_leftMinCps"] = settings::KA_leftMinCps;
	j["KA_leftMaxCps"] = settings::KA_leftMaxCps;
	j["KA_smooth"] = settings::KA_smooth;
	j["KA_targetPriority"] = settings::KA_targetPriority;
	j["KA_mousePressCheck"] = settings::KA_mousePressCheck;
	j["KA_weaponOnly"] = settings::KA_weaponOnly;
	j["KA_swordBlock"] = settings::KA_swordBlock;
	j["KA_allowInventory"] = settings::KA_allowInventory;
	j["KA_mouseMoveCheck"] = settings::KA_mouseMoveCheck;
	j["KA_visibilityCheck"] = settings::KA_visibilityCheck;
	j["KA_invisibleCheck"] = settings::KA_invisibleCheck;
	j["KA_ignoreFriends"] = settings::KA_ignoreFriends;
	j["KA_leftMinCps"] = settings::KA_leftMinCps;
	j["KA_leftMaxCps"] = settings::KA_leftMaxCps;
	j["KA_targetPriority"] = settings::KA_targetPriority;

	// Nametags
	j["NT_Enabled"] = settings::NT_Enabled;
	j["NT_Key"] = settings::NT_Key;
	j["NT_TextSize"] = settings::NT_TextSize;
	j["NT_TextColor"] = settings::NT_TextColor;
	j["NT_TextOutline"] = settings::NT_TextOutline;
	j["NT_TextOutlineColor"] = settings::NT_TextOutlineColor;
	j["NT_TextUnrenderDistance"] = settings::NT_TextUnrenderDistance;
	j["NT_FadeDistance"] = settings::NT_FadeDistance;
	j["NT_Background"] = settings::NT_Background;
	j["NT_BackgroundColor"] = settings::NT_BackgroundColor;
	j["NT_BackgroundOutline"] = settings::NT_BackgroundOutline;
	j["NT_BackgroundOutlineColor"] = settings::NT_BackgroundOutlineColor;
	j["NT_MultiLine"] = settings::NT_MultiLine;
	j["NT_DisplayHealth"] = settings::NT_DisplayHealth;
	j["NT_DisplayDistance"] = settings::NT_DisplayDistance;
	j["NT_DisplayInvisible"] = settings::NT_DisplayInvisible;

	// Array List
	j["AL_Enabled"] = settings::AL_Enabled;
	j["AL_Key"] = settings::AL_Key;
	j["AL_renderPosition"] = settings::AL_renderPosition;
	j["AL_textSize"] = settings::AL_textSize;
	j["AL_textColor"] = settings::AL_textColor;
	j["AL_backgroundPadding"] = settings::AL_backgroundPadding;
	j["AL_backgroundColor"] = settings::AL_backgroundColor;
	j["AL_colorMode"] = settings::AL_colorMode;
	

	// Radar
	j["Radar_Enabled"] = settings::Radar_Enabled;
	j["Radar_Key"] = settings::Radar_Key;
	j["Radar_Radius"] = settings::Radar_Radius;
	j["Radar_RotateWithPlayer"] = settings::Radar_RotateWithPlayer;
	j["Radar_ShowNames"] = settings::Radar_ShowNames;
	j["Radar_Size"] = settings::Radar_Size;
	j["Radar_SquareRoundness"] = settings::Radar_SquareRoundness;
	j["Radar_Position"] = settings::Radar_Position;
	j["Radar_LocalPlayerColor"] = settings::Radar_LocalPlayerColor;
	j["Radar_PlayerColor"] = settings::Radar_PlayerColor;
	j["Radar_FriendColor"] = settings::Radar_FriendColor;
	j["Radar_BackgroundColor"] = settings::Radar_BackgroundColor;

	// Aim Assist
	j["AA_Enabled"] = settings::AA_Enabled;
	j["AA_Key"] = settings::AA_Key;
	j["AA_visibilityCheck"] = settings::AA_visibilityCheck;
	j["AA_sprintCheck"] = settings::AA_sprintCheck;
	j["AA_blockBreakCheck"] = settings::AA_blockBreakCheck;
	j["AA_weaponOnly"] = settings::AA_weaponOnly;
	j["AA_invisibleCheck"] = settings::AA_invisibleCheck;
	j["AA_mousePressCheck"] = settings::AA_mousePressCheck;
	j["AA_mouseMoveCheck"] = settings::AA_mouseMoveCheck;
	j["AA_aimAssistFeedback"] = settings::AA_aimAssistFeedback;
	j["AA_aimAssistFeedbackColor"] = settings::AA_aimAssistFeedbackColor;
	j["AA_fovCircle"] = settings::AA_fovCircle;
	j["AA_fovCircleColor"] = settings::AA_fovCircleColor;
	j["AA_adaptive"] = settings::AA_adaptive;
	j["AA_adaptiveOffset"] = settings::AA_adaptiveOffset;
	j["AA_fov"] = settings::AA_fov;
	j["AA_smooth"] = settings::AA_smooth;
	j["AA_aimDistance"] = settings::AA_aimDistance;
	j["AA_randomYaw"] = settings::AA_randomYaw;
	j["AA_randomPitch"] = settings::AA_randomPitch;
	j["AA_targetPriority"] = settings::AA_targetPriority;
	j["AA_ignoreFriends"] = settings::AA_ignoreFriends;

	// Reach
	j["Reach_Enabled"] = settings::Reach_Enabled;
	j["Reach_Key"] = settings::Reach_Key;
	j["Reach_ReachDistance"] = settings::Reach_ReachDistance;

	// Sprint Reset
	j["SR_Enabled"] = settings::SR_Enabled;
	j["SR_Key"] = settings::SR_Key;
	j["SR_DelayBetween"] = settings::SR_DelayBetween;
	j["SR_LetGoDelay"] = settings::SR_LetGoDelay;

	// Sprint
	j["Sprint_Enabled"] = settings::S_Enabled;
	j["Sprint_Key"] = settings::S_Key;

	// Left Auto Clicker
	j["LAC_Enabled"] = settings::LAC_Enabled;
	j["LAC_Key"] = settings::LAC_Key;
	j["LAC_leftMaxCps"] = settings::LAC_leftMaxCps;
	j["LAC_leftMinCps"] = settings::LAC_leftMinCps;
	j["LAC_ignoreBlocks"] = settings::LAC_ignoreBlocks;
	j["LAC_swordBlock"] = settings::LAC_swordBlock;
	j["LAC_weaponOnly"] = settings::LAC_weaponOnly;
	j["LAC_allowInventory"] = settings::LAC_allowInventory;
	j["LAC_inventoryMultiplier"] = settings::LAC_inventoryMultiplier;
	j["LAC_advancedMode"] = settings::LAC_advancedMode;
	j["LAC_dropChance"] = settings::LAC_dropChance;
	j["LAC_spikeChance"] = settings::LAC_spikeChance;
	j["LAC_spikeMultiplier"] = settings::LAC_spikeMultiplier;
	j["LAC_kurtosis"] = settings::LAC_kurtosis;
	j["LAC_burstEnabled"] = settings::LAC_burstEnabled;
	j["LAC_burstChance"] = settings::LAC_burstChance;

	// Right Auto Clicker
	j["RAC_Enabled"] = settings::RAC_Enabled;
	j["RAC_Key"] = settings::RAC_Key;
	j["RAC_rightMaxCps"] = settings::RAC_rightMaxCps;
	j["RAC_rightMinCps"] = settings::RAC_rightMinCps;
	j["RAC_blocksOnly"] = settings::RAC_blocksOnly;

	// Bridge Assist
	j["BA_Enabled"] = settings::BA_Enabled;
	j["BA_Key"] = settings::BA_Key;
	j["BA_OnlyOnShift"] = settings::BA_OnlyOnShift;
	j["BA_IgnoreForwardsMovement"] = settings::BA_IgnoreForwardsMovement;
	j["BA_AutoSwap"] = settings::BA_AutoSwap;
	j["BA_PitchCheck"] = settings::BA_PitchCheck;
	j["BA_BlockCheck"] = settings::BA_BlockCheck;

	// Velocity
	j["Velocity_Enabled"] = settings::Velocity_Enabled;
	j["Velocity_Key"] = settings::Velocity_Key;
	j["Velocity_Mode"] = settings::Velocity_Mode;
	j["Velocity_JRReactionTime"] = settings::Velocity_JRReactionTime;
	j["Velocity_JRChange"] = settings::Velocity_JRChange;

	// Chest Stealer
	j["CS_Enabled"] = settings::CS_Enabled;
	j["CS_Delay"] = settings::CS_Delay;
	j["CS_Key"] = settings::CS_Key;
	j["CS_Items"] = settings::CS_Items;

	// Client Brand Changer
	j["CBC_Enabled"] = settings::CBC_Enabled;
	j["CBC_Key"] = settings::CBC_Key;
	j["CBC_ClientBrand"] = settings::CBC_ClientBrand;

	// Block Reach
	j["BR_Enabled"] = settings::BR_Enabled;
	j["BR_Key"] = settings::BR_Key;
	j["BR_ReachDistance"] = settings::BR_ReachDistance;

	// Weapon
	j["Weapon_Sword"] = settings::Weapon_Sword;
	j["Weapon_Axe"] = settings::Weapon_Axe;
	j["Weapon_Stick"] = settings::Weapon_Stick;
	j["Weapon_Fist"] = settings::Weapon_Fist;

	// Tag Back
	j["TB_Enabled"] = settings::TB_Enabled;
	j["TB_Key"] = settings::TB_Key;
	j["TB_visibilityCheck"] = settings::TB_visibilityCheck;
	j["TB_aimAssistFeedback"] = settings::TB_aimAssistFeedback;
	j["TB_aimAssistFeedbackColor"] = settings::TB_aimAssistFeedbackColor;
	j["TB_fovCircle"] = settings::TB_fovCircle;
	j["TB_fovCircleColor"] = settings::TB_fovCircleColor;
	j["TB_adaptive"] = settings::TB_adaptive;
	j["TB_adaptiveOffset"] = settings::TB_adaptiveOffset;
	j["TB_fov"] = settings::TB_fov;
	j["TB_smooth"] = settings::TB_smooth;
	j["TB_randomYaw"] = settings::TB_randomYaw;
	j["TB_randomPitch"] = settings::TB_randomPitch;
	j["TB_targetPriority"] = settings::TB_targetPriority;
	j["TB_ignoreFriends"] = settings::TB_ignoreFriends;
	j["TB_autoClick"] = settings::TB_autoClick;
	j["TB_maxCps"] = settings::TB_maxCps;
	j["TB_minCps"] = settings::TB_minCps;

	// IT ESP
	j["ITESP_Enabled"] = settings::ITESP_Enabled;
	j["ITESP_Key"] = settings::ITESP_Key;
	j["ITESP_Text"] = settings::ITESP_Text;
	j["ITESP_TextSize"] = settings::ITESP_TextSize;
	j["ITESP_TextColor"] = settings::ITESP_TextColor;
	j["ITESP_TextOutline"] = settings::ITESP_TextOutline;
	j["ITESP_TextOutlineColor"] = settings::ITESP_TextOutlineColor;
	j["ITESP_TextUnrenderDistance"] = settings::ITESP_TextUnrenderDistance;
	j["ITESP_FadeDistance"] = settings::ITESP_FadeDistance;
	j["ITESP_HealthBar"] = settings::ITESP_HealthBar;
	j["ITESP_BoxType"] = settings::ITESP_BoxType;
	j["ITESP_3DBoxColor"] = settings::ITESP_3DBoxColor;
	j["ITESP_3DBoxThickness"] = settings::ITESP_3DBoxThickness;
	j["ITESP_Box"] = settings::ITESP_Box;
	j["ITESP_BoxColor"] = settings::ITESP_BoxColor;
	j["ITESP_FilledBox"] = settings::ITESP_FilledBox;
	j["ITESP_FilledBoxColor"] = settings::ITESP_FilledBoxColor;
	j["ITESP_SecondFilledBoxColor"] = settings::ITESP_SecondFilledBoxColor;
	j["ITESP_Outline"] = settings::ITESP_Outline;
	j["ITESP_OutlineColor"] = settings::ITESP_OutlineColor;
	j["ITESP_HighlightFriends"] = settings::ITESP_HighlightFriends;
	j["ITESP_Friend3DBoxColor"] = settings::ITESP_Friend3DBoxColor;
	j["ITESP_FriendBoxColor"] = settings::ITESP_FriendBoxColor;
	j["ITESP_FriendFilledBoxColor"] = settings::ITESP_FriendFilledBoxColor;
	j["ITESP_FriendSecondFilledBoxColor"] = settings::ITESP_FriendSecondFilledBoxColor;
	j["ITESP_FriendOutlineColor"] = settings::ITESP_FriendOutlineColor;

	return true;
}

bool ConfigManager::JsonToSettings(const json& j)
{
	// Menu
	ConfigManager::GetJsonValue(j, settings::Menu_GUIMovement, "Menu_GUIMovement");
	ConfigManager::GetJsonValue(j, settings::Menu_ShowHiddenCategories, "Menu_ShowHiddenCategories");

	// Hud
	ConfigManager::GetJsonValue(j, settings::Hud_DisableAllRendering, "Hud_DisableAllRendering");
	ConfigManager::GetJsonValue(j, settings::Hud_Watermark, "Hud_Watermark");
	ConfigManager::GetJsonValue(j, settings::Hud_WatermarkPosition, "Hud_WatermarkPosition");
	ConfigManager::GetJsonValue(j, settings::Hud_WatermarkVersion, "Hud_WatermarkVersion");
	ConfigManager::GetJsonValue(j, settings::Hud_WatermarkFps, "Hud_WatermarkFps");
	ConfigManager::GetJsonValue(j, settings::Hud_WatermarkPing, "Hud_WatermarkPing");
	ConfigManager::GetJsonValue(j, settings::Hud_WatermarkCoords, "Hud_WatermarkCoords");
	ConfigManager::GetJsonValue(j, settings::Hud_WatermarkDirection, "Hud_WatermarkDirection");
	ConfigManager::GetJsonValue(j, settings::Hud_WatermarkTime, "Hud_WatermarkTime");
	ConfigManager::GetJsonValue(j, settings::Hud_ShowKeybinds, "Hud_ShowKeybinds");
	ConfigManager::GetJsonValue(j, settings::Hud_KeybindsPosition, "Hud_KeybindsPosition");

	// ESP
	ConfigManager::GetJsonValue(j, settings::ESP_Enabled, "ESP_Enabled");
	ConfigManager::GetJsonValue(j, settings::ESP_Key, "ESP_Key");
	ConfigManager::GetJsonValue(j, settings::ESP_FadeDistance, "ESP_FadeDistance");
	ConfigManager::GetJsonValue(j, settings::ESP_HealthBar, "ESP_HealthBar");
	ConfigManager::GetJsonValue(j, settings::ESP_BoxType, "ESP_BoxType");
	ConfigManager::GetJsonValue(j, settings::ESP_3DBoxColor, "ESP_3DBoxColor");
	ConfigManager::GetJsonValue(j, settings::ESP_3DBoxThickness, "ESP_3DBoxThickness");
	ConfigManager::GetJsonValue(j, settings::ESP_Box, "ESP_Box");
	ConfigManager::GetJsonValue(j, settings::ESP_BoxColor, "ESP_BoxColor");
	ConfigManager::GetJsonValue(j, settings::ESP_FilledBox, "ESP_FilledBox");
	ConfigManager::GetJsonValue(j, settings::ESP_FilledBoxColor, "ESP_FilledBoxColor");
	ConfigManager::GetJsonValue(j, settings::ESP_SecondFilledBoxColor, "ESP_SecondFilledBoxColor");
	ConfigManager::GetJsonValue(j, settings::ESP_Outline, "ESP_Outline");
	ConfigManager::GetJsonValue(j, settings::ESP_OutlineColor, "ESP_OutlineColor");
	ConfigManager::GetJsonValue(j, settings::ESP_HighlightFriends, "ESP_HighlightFriends");
	ConfigManager::GetJsonValue(j, settings::ESP_Friend3DBoxColor, "ESP_Friend3DBoxColor");
	ConfigManager::GetJsonValue(j, settings::ESP_FriendBoxColor, "ESP_FriendBoxColor");
	ConfigManager::GetJsonValue(j, settings::ESP_FriendFilledBoxColor, "ESP_FriendFilledBoxColor");
	ConfigManager::GetJsonValue(j, settings::ESP_FriendSecondFilledBoxColor, "ESP_FriendSecondFilledBoxColor");
	ConfigManager::GetJsonValue(j, settings::ESP_FriendOutlineColor, "ESP_FriendOutlineColor");

	// Nametags
	ConfigManager::GetJsonValue(j, settings::NT_Enabled, "NT_Enabled");
	ConfigManager::GetJsonValue(j, settings::NT_Key, "NT_Key");
	ConfigManager::GetJsonValue(j, settings::NT_TextSize, "NT_TextSize");
	ConfigManager::GetJsonValue(j, settings::NT_TextColor, "NT_TextColor");
	ConfigManager::GetJsonValue(j, settings::NT_TextOutline, "NT_TextOutline");
	ConfigManager::GetJsonValue(j, settings::NT_TextOutlineColor, "NT_TextOutlineColor");
	ConfigManager::GetJsonValue(j, settings::NT_TextUnrenderDistance, "NT_TextUnrenderDistance");
	ConfigManager::GetJsonValue(j, settings::NT_FadeDistance, "NT_FadeDistance");
	ConfigManager::GetJsonValue(j, settings::NT_Background, "NT_Background");
	ConfigManager::GetJsonValue(j, settings::NT_BackgroundColor, "NT_BackgroundColor");
	ConfigManager::GetJsonValue(j, settings::NT_BackgroundOutline, "NT_BackgroundOutline");
	ConfigManager::GetJsonValue(j, settings::NT_BackgroundOutlineColor, "NT_BackgroundOutlineColor");
	ConfigManager::GetJsonValue(j, settings::NT_MultiLine, "NT_MultiLine");
	ConfigManager::GetJsonValue(j, settings::NT_DisplayHealth, "NT_DisplayHealth");
	ConfigManager::GetJsonValue(j, settings::NT_DisplayDistance, "NT_DisplayDistance");
	ConfigManager::GetJsonValue(j, settings::NT_DisplayInvisible, "NT_DisplayInvisible");

	// Array List
	ConfigManager::GetJsonValue(j, settings::AL_Enabled, "AL_Enabled");
	ConfigManager::GetJsonValue(j, settings::AL_Key, "AL_Key");
	ConfigManager::GetJsonValue(j, settings::AL_renderPosition, "AL_renderPosition");
	ConfigManager::GetJsonValue(j, settings::AL_textSize, "AL_textSize");
	ConfigManager::GetJsonValue(j, settings::AL_textColor, "AL_textColor");
	ConfigManager::GetJsonValue(j, settings::AL_backgroundPadding, "AL_backgroundPadding");
	ConfigManager::GetJsonValue(j, settings::AL_backgroundColor, "AL_backgroundColor");
	ConfigManager::GetJsonValue(j, settings::AL_colorMode, "AL_colorMode");

	// Radar
	ConfigManager::GetJsonValue(j, settings::Radar_Enabled, "Radar_Enabled");
	ConfigManager::GetJsonValue(j, settings::Radar_Key, "Radar_Key");
	ConfigManager::GetJsonValue(j, settings::Radar_Radius, "Radar_Radius");
	ConfigManager::GetJsonValue(j, settings::Radar_RotateWithPlayer, "Radar_RotateWithPlayer");
	ConfigManager::GetJsonValue(j, settings::Radar_ShowNames, "Radar_ShowNames");
	ConfigManager::GetJsonValue(j, settings::Radar_Size, "Radar_Size");
	ConfigManager::GetJsonValue(j, settings::Radar_SquareRoundness, "Radar_SquareRoundness");
	ConfigManager::GetJsonValue(j, settings::Radar_Position, "Radar_Position");
	ConfigManager::GetJsonValue(j, settings::Radar_LocalPlayerColor, "Radar_LocalPlayerColor");
	ConfigManager::GetJsonValue(j, settings::Radar_PlayerColor, "Radar_PlayerColor");
	ConfigManager::GetJsonValue(j, settings::Radar_FriendColor, "Radar_FriendColor");
	ConfigManager::GetJsonValue(j, settings::Radar_BackgroundColor, "Radar_BackgroundColor");

	// Aim Assist
	ConfigManager::GetJsonValue(j, settings::AA_Enabled, "AA_Enabled");
	ConfigManager::GetJsonValue(j, settings::AA_Key, "AA_Key");
	ConfigManager::GetJsonValue(j, settings::AA_visibilityCheck, "AA_visibilityCheck");
	ConfigManager::GetJsonValue(j, settings::AA_sprintCheck, "AA_sprintCheck");
	ConfigManager::GetJsonValue(j, settings::AA_blockBreakCheck, "AA_blockBreakCheck");
	ConfigManager::GetJsonValue(j, settings::AA_weaponOnly, "AA_weaponOnly");
	ConfigManager::GetJsonValue(j, settings::AA_invisibleCheck, "AA_invisibleCheck");
	ConfigManager::GetJsonValue(j, settings::AA_mousePressCheck, "AA_mousePressCheck");
	ConfigManager::GetJsonValue(j, settings::AA_mouseMoveCheck, "AA_mouseMoveCheck");
	ConfigManager::GetJsonValue(j, settings::AA_aimAssistFeedback, "AA_aimAssistFeedback");
	ConfigManager::GetJsonValue(j, settings::AA_aimAssistFeedbackColor, "AA_aimAssistFeedbackColor");
	ConfigManager::GetJsonValue(j, settings::AA_fovCircle, "AA_fovCircle");
	ConfigManager::GetJsonValue(j, settings::AA_fovCircleColor, "AA_fovCircleColor");
	ConfigManager::GetJsonValue(j, settings::AA_adaptive, "AA_adaptive");
	ConfigManager::GetJsonValue(j, settings::AA_adaptiveOffset, "AA_adaptiveOffset");
	ConfigManager::GetJsonValue(j, settings::AA_fov, "AA_fov");
	ConfigManager::GetJsonValue(j, settings::AA_smooth, "AA_smooth");
	ConfigManager::GetJsonValue(j, settings::AA_aimDistance, "AA_aimDistance");
	ConfigManager::GetJsonValue(j, settings::AA_randomYaw, "AA_randomYaw");
	ConfigManager::GetJsonValue(j, settings::AA_randomPitch, "AA_randomPitch");
	ConfigManager::GetJsonValue(j, settings::AA_targetPriority, "AA_targetPriority");
	ConfigManager::GetJsonValue(j, settings::AA_ignoreFriends, "AA_ignoreFriends");

	// Reach
	ConfigManager::GetJsonValue(j, settings::Reach_Enabled, "Reach_Enabled");
	ConfigManager::GetJsonValue(j, settings::Reach_Key, "Reach_Key");
	ConfigManager::GetJsonValue(j, settings::Reach_ReachDistance, "Reach_ReachDistance");

	// Sprint Reset
	ConfigManager::GetJsonValue(j, settings::SR_Enabled, "SR_Enabled");
	ConfigManager::GetJsonValue(j, settings::SR_Key, "SR_Key");
	ConfigManager::GetJsonValue(j, settings::SR_DelayBetween, "SR_DelayBetween");
	ConfigManager::GetJsonValue(j, settings::SR_LetGoDelay, "SR_LetGoDelay");

	// Sprint
	ConfigManager::GetJsonValue(j, settings::S_Enabled, "S_Enabled");
	ConfigManager::GetJsonValue(j, settings::S_Key, "S_Key");

	// Left Auto Clicker
	ConfigManager::GetJsonValue(j, settings::LAC_Enabled, "LAC_Enabled");
	ConfigManager::GetJsonValue(j, settings::LAC_Key, "LAC_Key");
	ConfigManager::GetJsonValue(j, settings::LAC_leftMaxCps, "LAC_leftMaxCps");
	ConfigManager::GetJsonValue(j, settings::LAC_leftMinCps, "LAC_leftMinCps");
	ConfigManager::GetJsonValue(j, settings::LAC_ignoreBlocks, "LAC_ignoreBlocks");
	ConfigManager::GetJsonValue(j, settings::LAC_swordBlock, "LAC_swordBlock");
	ConfigManager::GetJsonValue(j, settings::LAC_weaponOnly, "LAC_weaponOnly");
	ConfigManager::GetJsonValue(j, settings::LAC_allowInventory, "LAC_allowInventory");
	ConfigManager::GetJsonValue(j, settings::LAC_inventoryMultiplier, "LAC_inventoryMultiplier");
	ConfigManager::GetJsonValue(j, settings::LAC_advancedMode, "LAC_advancedMode");
	ConfigManager::GetJsonValue(j, settings::LAC_dropChance, "LAC_dropChance");
	ConfigManager::GetJsonValue(j, settings::LAC_spikeChance, "LAC_spikeChance");
	ConfigManager::GetJsonValue(j, settings::LAC_spikeMultiplier, "LAC_spikeMultiplier");
	ConfigManager::GetJsonValue(j, settings::LAC_kurtosis, "LAC_kurtosis");
	ConfigManager::GetJsonValue(j, settings::LAC_burstEnabled, "LAC_burstEnabled");
	ConfigManager::GetJsonValue(j, settings::LAC_burstChance, "LAC_burstChance");

	// Right Auto Clicker
	ConfigManager::GetJsonValue(j, settings::RAC_Enabled, "RAC_Enabled");
	ConfigManager::GetJsonValue(j, settings::RAC_Key, "RAC_Key");
	ConfigManager::GetJsonValue(j, settings::RAC_rightMaxCps, "RAC_rightMaxCps");
	ConfigManager::GetJsonValue(j, settings::RAC_rightMinCps, "RAC_rightMinCps");
	ConfigManager::GetJsonValue(j, settings::RAC_blocksOnly, "RAC_blocksOnly");

	// Bridge Assist
	ConfigManager::GetJsonValue(j, settings::BA_Enabled, "BA_Enabled");
	ConfigManager::GetJsonValue(j, settings::BA_Key, "BA_Key");
	ConfigManager::GetJsonValue(j, settings::BA_OnlyOnShift, "BA_OnlyOnShift");
	ConfigManager::GetJsonValue(j, settings::BA_IgnoreForwardsMovement, "BA_IgnoreForwardsMovement");
	ConfigManager::GetJsonValue(j, settings::BA_AutoSwap, "BA_AutoSwap");
	ConfigManager::GetJsonValue(j, settings::BA_PitchCheck, "BA_PitchCheck");
	ConfigManager::GetJsonValue(j, settings::BA_BlockCheck, "BA_BlockCheck");

	// no hit delay
	ConfigManager::GetJsonValue(j, settings::NHD_Enabled, "NHD_Enabled");

	// sprint

	ConfigManager::GetJsonValue(j, settings::S_Enabled, "S_Enabled");

	// Velocity
	ConfigManager::GetJsonValue(j, settings::Velocity_Enabled, "Velocity_Enabled");
	ConfigManager::GetJsonValue(j, settings::Velocity_Key, "Velocity_Key");
	ConfigManager::GetJsonValue(j, settings::Velocity_Mode, "Velocity_Mode");
	ConfigManager::GetJsonValue(j, settings::Velocity_JRReactionTime, "Velocity_JRReactionTime");
	ConfigManager::GetJsonValue(j, settings::Velocity_JRChange, "Velocity_JRChange");

	// Chest Stealer
	ConfigManager::GetJsonValue(j, settings::CS_Enabled, "CS_Enabled");
	ConfigManager::GetJsonValue(j, settings::CS_Delay, "CS_Delay");
	ConfigManager::GetJsonValue(j, settings::CS_Key, "CS_Key");
	ConfigManager::GetJsonValue(j, settings::CS_Items, "CS_Items");

	// Client Brand Changer
	ConfigManager::GetJsonValue(j, settings::CBC_Enabled, "CBC_Enabled");
	ConfigManager::GetJsonValue(j, settings::CBC_Key, "CBC_Key");
	ConfigManager::GetJsonValue(j, settings::CBC_ClientBrand, "CBC_ClientBrand");

	// Block Reach
	ConfigManager::GetJsonValue(j, settings::BR_Enabled, "BR_Enabled");
	ConfigManager::GetJsonValue(j, settings::BR_Key, "BR_Key");
	ConfigManager::GetJsonValue(j, settings::BR_ReachDistance, "BR_ReachDistance");

	ConfigManager::GetJsonValue(j, settings::KA_Enabled, "KA_Enabled");
	ConfigManager::GetJsonValue(j, settings::KA_Key, "KA_Key");
	ConfigManager::GetJsonValue(j, settings::KA_range, "KA_range");
	ConfigManager::GetJsonValue(j, settings::KA_fov, "KA_fov");
	ConfigManager::GetJsonValue(j, settings::KA_advancedMode, "KA_advancedMode");
	ConfigManager::GetJsonValue(j, settings::KA_dropChance, "KA_dropChance");
	ConfigManager::GetJsonValue(j, settings::KA_spikeChance, "KA_spikeChance");
	ConfigManager::GetJsonValue(j, settings::KA_spikeMultiplier, "KA_spikeMultiplier");
	ConfigManager::GetJsonValue(j, settings::KA_kurtosis, "KA_kurtosis");
	ConfigManager::GetJsonValue(j, settings::KA_burstEnabled, "KA_burstEnabled");
	ConfigManager::GetJsonValue(j, settings::KA_burstChance, "KA_burstChance");
	ConfigManager::GetJsonValue(j, settings::KA_lockSmooth, "KA_lockSmooth");
	ConfigManager::GetJsonValue(j, settings::KA_jitterAmount, "KA_jitterAmount");
	ConfigManager::GetJsonValue(j, settings::KA_leftMinCps, "KA_leftMinCps");
	ConfigManager::GetJsonValue(j, settings::KA_leftMaxCps, "KA_leftMaxCps");
	ConfigManager::GetJsonValue(j, settings::KA_smooth, "KA_smooth");
	ConfigManager::GetJsonValue(j, settings::KA_targetPriority, "KA_targetPriority");
	ConfigManager::GetJsonValue(j, settings::KA_mousePressCheck, "KA_mousePressCheck");
	ConfigManager::GetJsonValue(j, settings::KA_weaponOnly, "KA_weaponOnly");
	ConfigManager::GetJsonValue(j, settings::KA_swordBlock, "KA_swordBlock");
	ConfigManager::GetJsonValue(j, settings::KA_allowInventory, "KA_allowInventory");
	ConfigManager::GetJsonValue(j, settings::KA_mouseMoveCheck, "KA_mouseMoveCheck");
	ConfigManager::GetJsonValue(j, settings::KA_visibilityCheck, "KA_visibilityCheck");
	ConfigManager::GetJsonValue(j, settings::KA_invisibleCheck, "KA_invisibleCheck");
	ConfigManager::GetJsonValue(j, settings::KA_ignoreFriends, "KA_ignoreFriends");

	
	// Weapon
	ConfigManager::GetJsonValue(j, settings::Weapon_Sword, "Weapon_Sword");
	ConfigManager::GetJsonValue(j, settings::Weapon_Axe, "Weapon_Axe");
	ConfigManager::GetJsonValue(j, settings::Weapon_Stick, "Weapon_Stick");
	ConfigManager::GetJsonValue(j, settings::Weapon_Fist, "Weapon_Fist");

	// Tag Back
	ConfigManager::GetJsonValue(j, settings::TB_Enabled, "TB_Enabled");
	ConfigManager::GetJsonValue(j, settings::TB_Key, "TB_Key");
	ConfigManager::GetJsonValue(j, settings::TB_visibilityCheck, "TB_visibilityCheck");
	ConfigManager::GetJsonValue(j, settings::TB_aimAssistFeedback, "TB_aimAssistFeedback");
	ConfigManager::GetJsonValue(j, settings::TB_aimAssistFeedbackColor, "TB_aimAssistFeedbackColor");
	ConfigManager::GetJsonValue(j, settings::TB_fovCircle, "TB_fovCircle");
	ConfigManager::GetJsonValue(j, settings::TB_fovCircleColor, "TB_fovCircleColor");
	ConfigManager::GetJsonValue(j, settings::TB_adaptive, "TB_adaptive");
	ConfigManager::GetJsonValue(j, settings::TB_adaptiveOffset, "TB_adaptiveOffset");
	ConfigManager::GetJsonValue(j, settings::TB_fov, "TB_fov");
	ConfigManager::GetJsonValue(j, settings::TB_smooth, "TB_smooth");
	ConfigManager::GetJsonValue(j, settings::TB_randomYaw, "TB_randomYaw");
	ConfigManager::GetJsonValue(j, settings::TB_randomPitch, "TB_randomPitch");
	ConfigManager::GetJsonValue(j, settings::TB_targetPriority, "TB_targetPriority");
	ConfigManager::GetJsonValue(j, settings::TB_ignoreFriends, "TB_ignoreFriends");
	ConfigManager::GetJsonValue(j, settings::TB_autoClick, "TB_autoClick");
	ConfigManager::GetJsonValue(j, settings::TB_maxCps, "TB_maxCps");
	ConfigManager::GetJsonValue(j, settings::TB_minCps, "TB_minCps");

	// IT ESP
	ConfigManager::GetJsonValue(j, settings::ITESP_Enabled, "ITESP_Enabled");
	ConfigManager::GetJsonValue(j, settings::ITESP_Key, "ITESP_Key");
	ConfigManager::GetJsonValue(j, settings::ITESP_Text, "ITESP_Text");
	ConfigManager::GetJsonValue(j, settings::ITESP_TextSize, "ITESP_TextSize");
	ConfigManager::GetJsonValue(j, settings::ITESP_TextColor, "ITESP_TextColor");
	ConfigManager::GetJsonValue(j, settings::ITESP_TextOutline, "ITESP_TextOutline");
	ConfigManager::GetJsonValue(j, settings::ITESP_TextOutlineColor, "ITESP_TextOutlineColor");
	ConfigManager::GetJsonValue(j, settings::ITESP_TextUnrenderDistance, "ITESP_TextUnrenderDistance");
	ConfigManager::GetJsonValue(j, settings::ITESP_FadeDistance, "ITESP_FadeDistance");
	ConfigManager::GetJsonValue(j, settings::ITESP_HealthBar, "ITESP_HealthBar");
	ConfigManager::GetJsonValue(j, settings::ITESP_BoxType, "ITESP_BoxType");
	ConfigManager::GetJsonValue(j, settings::ITESP_3DBoxColor, "ITESP_3DBoxColor");
	ConfigManager::GetJsonValue(j, settings::ITESP_3DBoxThickness, "ITESP_3DBoxThickness");
	ConfigManager::GetJsonValue(j, settings::ITESP_Box, "ITESP_Box");
	ConfigManager::GetJsonValue(j, settings::ITESP_BoxColor, "ITESP_BoxColor");
	ConfigManager::GetJsonValue(j, settings::ITESP_FilledBox, "ITESP_FilledBox");
	ConfigManager::GetJsonValue(j, settings::ITESP_FilledBoxColor, "ITESP_FilledBoxColor");
	ConfigManager::GetJsonValue(j, settings::ITESP_SecondFilledBoxColor, "ITESP_SecondFilledBoxColor");
	ConfigManager::GetJsonValue(j, settings::ITESP_Outline, "ITESP_Outline");
	ConfigManager::GetJsonValue(j, settings::ITESP_OutlineColor, "ITESP_OutlineColor");
	ConfigManager::GetJsonValue(j, settings::ITESP_HighlightFriends, "ITESP_HighlightFriends");
	ConfigManager::GetJsonValue(j, settings::ITESP_Friend3DBoxColor, "ITESP_Friend3DBoxColor");
	ConfigManager::GetJsonValue(j, settings::ITESP_FriendBoxColor, "ITESP_FriendBoxColor");
	ConfigManager::GetJsonValue(j, settings::ITESP_FriendFilledBoxColor, "ITESP_FriendFilledBoxColor");
	ConfigManager::GetJsonValue(j, settings::ITESP_FriendSecondFilledBoxColor, "ITESP_FriendSecondFilledBoxColor");
	ConfigManager::GetJsonValue(j, settings::ITESP_FriendOutlineColor, "ITESP_FriendOutlineColor");

	// Disable Chest Saler if the version is 1.8.9
	if (Java::Version == MinecraftVersion::VANILLA_1_8_9)
		settings::CS_Enabled = false;

	return true;
}

std::string ConfigManager::GetDocumentsPath()
{
	char path[MAX_PATH];

	// Get the path to the Documents folder (CSIDL_PERSONAL refers to "My Documents")
	if (SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, path) == S_OK) {
		return std::string(path);
	}
	else {
		std::cerr << "Failed to get Documents folder." << std::endl;
		return "";
	}
}

std::string ConfigManager::GetConfigPath()
{
	std::string path = ConfigManager::GetSincaraPath() + "/configs/";

	if (!std::filesystem::exists(path))
		std::filesystem::create_directories(path);

	return path;
}

std::string ConfigManager::GetSincaraPath()
{
	std::string path = ConfigManager::GetDocumentsPath() + "/Sincara+/";

	if (!std::filesystem::exists(path))
		std::filesystem::create_directories(path);

	return path;
}

bool ConfigManager::LoadFriends()
{
	std::string filePath = ConfigManager::GetSincaraPath() + "friends.json";

	if (!std::filesystem::exists(filePath))
		return false;

	std::ifstream file(filePath);
	if (!file.is_open())
		return false;

	json j;
	file >> j;
	file.close();

	if (!j.is_array())
		return false;

	settings::friends.clear();
	for (const auto& friendName : j)
	{
		settings::friends.push_back(friendName.get<std::string>());
	}

	return true;
}

bool ConfigManager::SaveFriends()
{
	std::string filePath = ConfigManager::GetSincaraPath() + "friends.json";

	// Ensure the directory exists before writing
	std::string dirPath = ConfigManager::GetSincaraPath();
	if (!std::filesystem::exists(dirPath))
		std::filesystem::create_directories(dirPath);

	json j;
	for (const auto& friendName : settings::friends)
	{
		j.push_back(friendName);
	}

	std::ofstream file(filePath);
	if (!file.is_open())
		return false;

	file << j.dump(4);
	file.close();

	return true;
}


bool ConfigManager::AddFriend(const std::string& name)
{
	if (IsFriend(name))
		return false;

	settings::friends.push_back(name);

	return SaveFriends();
}

bool ConfigManager::RemoveFriend(const std::string& name)
{
	auto it = std::find(settings::friends.begin(), settings::friends.end(), name);

	if (it == settings::friends.end())
		return false;

	settings::friends.erase(it);

	return SaveFriends();
}

bool ConfigManager::IsFriend(const std::string& name)
{
	return std::find(settings::friends.begin(), settings::friends.end(), name) != settings::friends.end();
}
