#pragma once
#include <memory>
#include "ChiGraphics/Textures/FImage.h"
#include <glm/glm.hpp>

namespace CHISTUDIO
{

// Singleton pattern class for managing all images in the project
class ImageManager
{
public:
	static ImageManager& GetInstance()
	{
		static ImageManager instance;
		return instance;
	}

	ImageManager(const ImageManager&) = delete;
	void operator=(const ImageManager&) = delete;

	FImage* GetRenderResult() const { return RenderResult.get(); }
	void SetRenderResult(std::unique_ptr<FImage> InImage);
private:
	ImageManager() {}
	~ImageManager() {}

	std::unique_ptr<FImage> RenderResult;
};

}
