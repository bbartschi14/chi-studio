#pragma once

#include "ChiGraphics/Textures/ImageModifier.h"

namespace CHISTUDIO 
{


class ImageThresholdModifier: public IImageModifier
{

public:
	ImageThresholdModifier(float InBrightnessThreshold) : BrightnessThreshold(InBrightnessThreshold) {};

	void ApplyModifier(class FImage* InImageToModify) const override;
	bool RenderUI() override;
	std::string GetName() const override { return "Threshold"; }
	float GetUIHeight() const override { return 40.0f; }

	float BrightnessThreshold;
};

}
