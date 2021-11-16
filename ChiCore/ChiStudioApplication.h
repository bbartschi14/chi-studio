#pragma once

#include "ChiGraphics/Application.h"
#include "UI/WHierarchy.h"
#include "UI/WObjectProperties.h"
#include "UI/WEditMode.h"

namespace CHISTUDIO {

class ChiStudioApplication : public Application
{
public:
	ChiStudioApplication(const std::string& InAppName, glm::ivec2 InWindowSize);
	void SetupScene() override;
	void DrawGUI() override;

private:
	std::unique_ptr<WHierarchy> HierarchyWidget;
	std::unique_ptr<WObjectProperties> ObjectPropertiesWidget;
	std::unique_ptr<WEditMode> EditModeWidget;

};

}

