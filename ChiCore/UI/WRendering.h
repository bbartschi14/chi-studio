#pragma once

#include "IWidget.h"
#include <vector>
#include <memory>
#include <string>
namespace CHISTUDIO {

class WRendering : public IWidget
{

public:
	WRendering();

	void Render(Application& InApplication);

private:
	std::unique_ptr<class FTexture> DisplayTexture;
	std::string FileName;
	int RenderWidth;
	int RenderHeight;
	int MaxBounces;
	int SamplesPerPixel; // Corresponds to super sampling anti aliasing
};

}