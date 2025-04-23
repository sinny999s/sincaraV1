#pragma once
#include "moduleManager/moduleBase.h"
#include <util/math/geometry.h>

class ITEsp : public ModuleBase
{
public:
	void Update() override;
	void RenderOverlay() override;
	void RenderHud() override {};

	void RenderMenu() override;

	bool IsEnabled() override { return settings::ITESP_Enabled; }
	std::string GetName() override { return Name; }
	std::string GetCategory() override { return Category; }
	int GetKey() override { return settings::ITESP_Key; }

	void SetEnabled(bool enabled) override { settings::ITESP_Enabled = enabled; }
	void Toggle() override { settings::ITESP_Enabled = !settings::ITESP_Enabled; }

	struct Data {
		std::vector<Vector3> boxVerticies;
		std::string name;
		std::string distText;
		float dist;
		float opacityFadeFactor;
		std::vector<Vector3> boundingBoxVerticies;
	};

private:
	std::string Name = "IT ESP";
	std::string Category = "TnT Tag";

	std::vector<Data> renderData;
};

