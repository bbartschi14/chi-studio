#pragma once

#include "Modifier.h"

namespace CHISTUDIO {

	// Apply a given number of iterations of Catmull-Clark subdivision
	class SubdivisionSurfaceModifier : public IModifier
	{
	public:
		SubdivisionSurfaceModifier(int InNumberOfIterations) : NumberOfIterations(InNumberOfIterations) {};

		void ApplyModifier(class VertexObject* InObjectToModify) const override;
		bool RenderUI() override;
		std::string GetName() const override { return "Subdivision Surface"; }
		float GetUIHeight() const override { return 40.0f; }

		int NumberOfIterations;
	};

}

