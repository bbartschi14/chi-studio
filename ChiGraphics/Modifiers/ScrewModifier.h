#pragma once

#include "Modifier.h"
#include "ChiGraphics/External.h"
#include <memory>
#include "ChiGraphics/Meshes/VertexObject.h"

namespace CHISTUDIO {

	enum class EScrewAxis
	{
		X,
		Y,
		Z
	};

	// Take the boundary profile of the mesh and continuously extrude it into a helix
	class ScrewModifier : public IModifier
	{
	public:
		ScrewModifier() : AngleInDegrees(180.0f), Height(0.0f), Steps(20) , ScrewAxis(EScrewAxis::Y), MergeEndWithStart(false) {};

		void ApplyModifier(VertexObject* InObjectToModify) const override;
		bool RenderUI() override;
		std::string GetName() const override { return "Screw"; }
		float GetUIHeight() const override { return 110.0f; }

		float AngleInDegrees;
		float Height;
		int Steps;
		EScrewAxis ScrewAxis;
		bool MergeEndWithStart;
	private:
		// Takes a current boundary loop, extrudes it to the next screw step, then returns the new end loop. The transform matrix defines the per-step transformation
		std::shared_ptr<FBoundaryLoop> ExtrudeBoundaryLoop(VertexObject* InObjectToModify, FBoundaryLoop* InBoundaryLoop, glm::mat4 InTransformMatrix) const;

	};

}

