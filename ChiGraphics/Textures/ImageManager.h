#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <unordered_map>

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

	// Return how many materials are contained in the manager's library
	size_t GetNumberOfImages() const { return ImageLibrary.size(); }

	// Retrieve image names
	std::vector<std::pair<std::string, class FImage*>> GetImages() const;

	// Retrieve the name of image from the pointer
	std::string GetNameOfImage(class FImage* InImage) const;

	// Change the key of a image
	void RenameImage(std::string InOriginalName, std::string InNewName);

	// Get a image from the library by name
	class FImage* GetImage(std::string InName) const;

	// Create new image. Returns the name of the image with a pointer to it
	std::pair<std::string, class FImage*> CreateNewImage(std::string InDesiredName = "Default");

	// Add an existing image to the library. Returns the name assigned to the image
	std::string AddImage(std::unique_ptr<class FImage> InImage, std::string InDesiredName);

	void BeginImportImage();

	std::pair<std::string, class FImage*> ImportImage(std::string imageFilename);

	class FImage* GetRenderResult() const { return RenderResult.get(); }
	void SetRenderResult(std::unique_ptr<class FImage> InImage);
private:
	// Find a unique default name for the library
	std::string GetUniqueName(std::string InBaseName) const;

	// Check if a name is unique to the library
	bool IsNameUnique(std::string InName) const;

	// Maps names to the images
	std::unordered_map<std::string, std::unique_ptr<class FImage>> ImageLibrary;

	ImageManager() {}
	~ImageManager() {}

	std::unique_ptr<class FImage> RenderResult;
};

}
