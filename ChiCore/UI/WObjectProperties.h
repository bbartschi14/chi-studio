#pragma once

#include "IWidget.h"
#include <vector>
#include "ChiGraphics/SceneNode.h"
#include "ChiGraphics/Components/MaterialComponent.h"
#include "ChiGraphics/Components/LightComponent.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/Components/CameraComponent.h"

namespace CHISTUDIO {

	class WObjectProperties : public IWidget
	{
	public:
		WObjectProperties();

		void Render(Application& InApplication) override;

	private:
		void RenderTransformPanel(std::vector<SceneNode*> selectedNodes);
		void RenderComponents(std::vector<SceneNode*> selectedNodes, Application& InApplication);

		// Component Panels
		void RenderLightComponent(std::vector<SceneNode*> selectedNodes, LightComponent* lightComponent);
		void RenderRenderingComponent(std::vector<SceneNode*> selectedNodes, RenderingComponent* renderingComponent, Application& InApplication);
		void RenderCameraComponent(std::vector<SceneNode*> selectedNodes, CameraComponent* cameraComponent);
		void RenderMaterialComponent(std::vector<SceneNode*> selectedNodes, MaterialComponent* materialComponent);
	};

}