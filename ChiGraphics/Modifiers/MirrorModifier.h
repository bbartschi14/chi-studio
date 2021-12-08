#pragma once

#include "Modifier.h"
#include "ChiGraphics/External.h"

namespace CHISTUDIO {

	// Copy prims by mirroring across an axis
	class MirrorModifier : public IModifier
	{
	public:
		MirrorModifier() : MirrorX(true), MirrorY(false), MirrorZ(false) {};

		void ApplyModifier(class VertexObject* InObjectToModify) const override;
		bool RenderUI() override;
		std::string GetName() const override { return "Mirror"; }
		float GetUIHeight() const override { return 40.0f; }

		bool MirrorX;
		bool MirrorY;
		bool MirrorZ;

	private:
		void ApplyMirrorFromReflectionMatrix(VertexObject* InObjectToModify, glm::mat4 InReflectionMatrix) const;
	};

}

