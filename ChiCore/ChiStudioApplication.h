#pragma once

#include "ChiGraphics/Application.h"
#include "UI/WHierarchy.h"
#include "UI/WObjectProperties.h"
#include "UI/WEditMode.h"
#include "UI/WRendering.h"
#include "UI/WTimeline.h"
#include "UI/WImageCompositor.h"

namespace CHISTUDIO {

class ChiStudioApplication : public Application
{
public:
	ChiStudioApplication(const std::string& InAppName, glm::ivec2 InWindowSize);
	void SetupScene(bool InIncludeDefaults) override;
	void DrawGUI(float InDeltaTime) override;

	WRendering* GetRenderingWidgetPtr() const { return RenderingWidget.get(); }
	WImageCompositor* GetImageCompositingWidgetPtr() const { return ImageCompositorWidget.get(); }

private:
	std::unique_ptr<WHierarchy> HierarchyWidget;
	std::unique_ptr<WObjectProperties> ObjectPropertiesWidget;
	std::unique_ptr<WEditMode> EditModeWidget;
	std::unique_ptr<WRendering> RenderingWidget;
	std::unique_ptr<WTimeline> TimelineWidget;
	std::unique_ptr<WImageCompositor> ImageCompositorWidget;

};

}

