#pragma once

#include "IWidget.h"
#include <vector>
#include <memory>

namespace CHISTUDIO {

class WRendering : public IWidget
{

public:
	WRendering();

	void Render(Application& InApplication);

private:
	std::unique_ptr<class FTexture> DisplayTexture;
	int RenderWidth;
	int RenderHeight;
};

}