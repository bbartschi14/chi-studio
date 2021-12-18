#pragma once
#include <memory>

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

	class FImage* GetRenderResult() const { return RenderResult.get(); }
	void SetRenderResult(std::unique_ptr<class FImage> InImage);
private:
	ImageManager() {}
	~ImageManager() {}

	std::unique_ptr<class FImage> RenderResult;
};

}
