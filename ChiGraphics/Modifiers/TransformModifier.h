#pragma once

#include "Modifier.h"
#include "ChiGraphics/External.h"

namespace CHISTUDIO {

	// Transform vertices procedurally
	class TransformModifier : public IModifier
	{
	public:
		TransformModifier() : Translation(glm::vec3(0.0f)) {};

		void ApplyModifier(class VertexObject* InObjectToModify) const override;
		bool RenderUI() override;
		std::string GetName() const override { return "Transform"; }
		float GetUIHeight() const override { return 40.0f; }

		glm::vec3 Translation;
	};

}

