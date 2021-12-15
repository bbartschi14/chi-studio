#pragma once

#include "IWidget.h"
#include <vector>
#include <memory>
#include <string>
#include "ChiGraphics/External.h"

namespace CHISTUDIO {

class WRendering : public IWidget
{

public:
	WRendering();

	void Render(Application& InApplication, float InDeltaTime);

private:
	std::unique_ptr<class FTexture> DisplayTexture;
	std::unique_ptr<class FTexture> HDRITexture;

	std::string FileName;
	int RenderWidth;
	int RenderHeight;
	int MaxBounces;
	int SamplesPerPixel; // Corresponds to super sampling anti aliasing
	std::shared_ptr<class FImage> HDRI;
	bool bUseHDRI;
	glm::vec3 BackgroundColor;
	float ResultZoomScale;
	int AnimationStartFrame;
	int AnimationEndFrame;
};

}