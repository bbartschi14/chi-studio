#define NOMINMAX

#include "ImageBloomModifier.h"
#include "ChiGraphics/Textures/FImage.h"
#include "ChiCore/UI/UILibrary.h"
#include <functional>
#include "ChiGraphics/Utilities.h"
namespace CHISTUDIO
{

void ImageBloomModifier::ApplyModifier(FImage* InImageToModify) const
{
	auto maskImage = make_unique<FImage>(64, 64);
	maskImage = FImage::MakeImageCopy(InImageToModify);

	auto thresholdFunction = [&](glm::vec3 InPixel)
	{
		float brightness = (InPixel.r + InPixel.g + InPixel.b) / 3.0f;
		return brightness >= BrightnessThreshold;
	};

	maskImage->MaskPixels(thresholdFunction);
	maskImage->ApplyGaussianBlur(BlurIterations);

	InImageToModify->AdditiveBlend(maskImage.get());
}

bool ImageBloomModifier::RenderUI()
{
	bool wasModified = false;
	if (ImGui::SliderFloat("Brightness Threshold", &BrightnessThreshold, 0.0f, 3.0f))
	{
		wasModified = true;
	}
	if (ImGui::SliderInt("Iterations", &BlurIterations, 0, 50))
	{
		wasModified = true;
	}
	return wasModified;
}

}
