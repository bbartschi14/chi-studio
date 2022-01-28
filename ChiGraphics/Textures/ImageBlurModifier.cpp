#define NOMINMAX

#include "ImageBlurModifier.h"
#include "ChiGraphics/Textures/FImage.h"
#include "ChiCore/UI/UILibrary.h"

namespace CHISTUDIO
{

void ImageBlurModifier::ApplyModifier(FImage* InImageToModify) const
{
	InImageToModify->ApplyGaussianBlur(NumberOfIterations);
}

bool ImageBlurModifier::RenderUI()
{
	bool wasModified = false;
	if (ImGui::SliderInt("Iterations", &NumberOfIterations, 0, 50))
	{
		wasModified = true;
	}
	return wasModified;
}

}
