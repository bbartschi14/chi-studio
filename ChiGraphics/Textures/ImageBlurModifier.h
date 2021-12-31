#pragma once

#include "ChiGraphics/Textures/ImageModifier.h"

namespace CHISTUDIO 
{


class ImageBlurModifier: public IImageModifier
{

public:
	ImageBlurModifier(int InNumberOfIterations) : NumberOfIterations(InNumberOfIterations) {};

	void ApplyModifier(class FImage* InImageToModify) const override;
	bool RenderUI() override;
	std::string GetName() const override { return "Blur"; }
	float GetUIHeight() const override { return 40.0f; }

	int NumberOfIterations;
};

}
