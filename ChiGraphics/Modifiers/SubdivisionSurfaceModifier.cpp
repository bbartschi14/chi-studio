#include "SubdivisionSurfaceModifier.h"
#include "ChiGraphics/Meshes/VertexObject.h"

void CHISTUDIO::SubdivisionSurfaceModifier::ApplyModifier(VertexObject* InObjectToModify) const
{
	for (int i = 0; i < NumberOfIterations; i++)
	{
		InObjectToModify->ApplySubdivisionSurface();
	}
}

bool CHISTUDIO::SubdivisionSurfaceModifier::RenderUI()
{
	bool wasModified = false;
	if (ImGui::SliderInt("Iterations", &NumberOfIterations, 0, 8))
	{
		wasModified = true;
	}
	return wasModified;
}
