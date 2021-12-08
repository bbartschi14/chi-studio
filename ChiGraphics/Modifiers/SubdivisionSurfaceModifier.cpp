#include "SubdivisionSurfaceModifier.h"
#include "ChiGraphics/Meshes/VertexObject.h"

namespace CHISTUDIO {

void SubdivisionSurfaceModifier::ApplyModifier(VertexObject* InObjectToModify) const
{
	for (int i = 0; i < NumberOfIterations; i++)
	{
		InObjectToModify->ApplySubdivisionSurface();
	}
}

bool SubdivisionSurfaceModifier::RenderUI()
{
	bool wasModified = false;
	if (ImGui::SliderInt("Iterations", &NumberOfIterations, 0, 8))
	{
		wasModified = true;
	}
	return wasModified;
}

}
