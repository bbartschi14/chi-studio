#pragma once

#include "IWidget.h"
#include <vector>
#include "ChiGraphics/SceneNode.h"
#include "ChiGraphics/Components/MaterialComponent.h"
#include "ChiGraphics/Components/LightComponent.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/Components/CameraComponent.h"

namespace CHISTUDIO {
	enum class EEditModeSelectionType;

	class WEditMode : public IWidget
	{
	public:
		WEditMode();

		void Render(Application& InApplication, float InDeltaTime) override;

	private:
		void RenderPrimitivesSection(std::vector<SceneNode*> selectedNodes, RenderingComponent* renderingComponent, Application& InApplication);

		float PrimNudgeAmount;
		glm::vec3 RotationOperationValue;
		glm::vec3 ScaleOperationValue;
		glm::vec3 StartingScaleOrigin;

		std::vector<glm::vec3> PreScaleVertexPosition;

	};

}