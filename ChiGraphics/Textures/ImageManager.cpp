#include "ImageManager.h"
#include "FImage.h"

namespace CHISTUDIO {

void ImageManager::SetRenderResult(std::unique_ptr<FImage> InImage)
{
	RenderResult = std::move(InImage);
}

}
