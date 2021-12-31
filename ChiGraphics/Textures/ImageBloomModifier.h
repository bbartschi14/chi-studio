#pragma once

#include "ChiGraphics/Textures/ImageModifier.h"

namespace CHISTUDIO 
{


class ImageBloomModifier : public IImageModifier
{

public:
	ImageBloomModifier(float InBrightnessThreshold, int InBlurIterations) : BrightnessThreshold(InBrightnessThreshold), BlurIterations(InBlurIterations) {};

	void ApplyModifier(class FImage* InImageToModify) const override;
	bool RenderUI() override;
	std::string GetName() const override { return "Bloom"; }
	float GetUIHeight() const override { return 60.0f; }

	float BrightnessThreshold;
	int BlurIterations;
};

}
