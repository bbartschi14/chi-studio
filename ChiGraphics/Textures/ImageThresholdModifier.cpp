#define NOMINMAX

#include "ImageThresholdModifier.h"
#include "ChiGraphics/Textures/FImage.h"
#include "ChiCore/UI/UILibrary.h"
#include <functional>

namespace CHISTUDIO
{

void ImageThresholdModifier::ApplyModifier(FImage* InImageToModify) const
{
	auto thresholdFunction = [&](glm::vec3 InPixel)
	{
		float brightness = (InPixel.r + InPixel.g + InPixel.b) / 3.0f;
		return brightness >= BrightnessThreshold;
	};

	InImageToModify->MaskPixels(thresholdFunction);
}

bool ImageThresholdModifier::RenderUI()
{
	bool wasModified = false;
	if (ImGui::SliderFloat("Brightness Threshold", &BrightnessThreshold, 0.0f, 3.0f))
	{
		wasModified = true;
	}
	return wasModified;
}

}
