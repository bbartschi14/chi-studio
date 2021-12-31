#pragma once

#include "ChiGraphics/Textures/ImageModifier.h"
#include <glm/glm.hpp>

namespace CHISTUDIO 
{

// http://filmicworlds.com/blog/filmic-tonemapping-operators/
enum class ETonemappingAlgorithm
{
	Linear,
	Reinhard,
	HaarmPeterDuiker, // Requires LUT
	HejlBurgessDawson, // Optimized version of HPD
	Hable // Used on Unchared 2 by John Hable
};

class ImageTonemappingModifier: public IImageModifier
{

public:
	ImageTonemappingModifier(ETonemappingAlgorithm InTonemappingAlgorithm, float InExposureBias) : 
		TonemappingAlgorithm(InTonemappingAlgorithm), 
		ExposureBias(InExposureBias) {};

	void ApplyModifier(class FImage* InImageToModify) const override;
	bool RenderUI() override;
	std::string GetName() const override { return "Tonemapping"; }
	float GetUIHeight() const override { return 70.0f; }

	glm::vec3 GetTonemappedValue(glm::vec3 InPixel) const;

	ETonemappingAlgorithm TonemappingAlgorithm;
	float ExposureBias;

private:
	glm::vec3 Uncharted2Tonemap(glm::vec3 InColor) const;
};

}
