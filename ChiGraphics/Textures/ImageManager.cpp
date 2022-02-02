#define NOMINMAX

#include "ImageManager.h"
#include "FImage.h"
#include "core.h"
#include "ChiGraphics/Utilities.h"
#include "ChiCore/UI/UILibrary.h"

namespace CHISTUDIO {

ImageManager::ImageManager() :
	RenderResult(make_unique<FImage>(512,512))
{

}

std::vector<std::pair<std::string, FImage*>> ImageManager::GetImages() const
{
	std::vector<std::pair<std::string, FImage*>> images;
	for (auto& pair : ImageLibrary)
	{
		images.push_back({ pair.first, pair.second.get() });
	}
	return images;
}

std::string ImageManager::GetNameOfImage(FImage* InImage) const
{
	for (auto& pair : ImageLibrary)
	{
		if (pair.second.get() == InImage)
		{
			return pair.first;
		}
	}
	return "";
}

void ImageManager::RenameImage(std::string InOriginalName, std::string InNewName)
{
	auto pair = ImageLibrary.find(InOriginalName);

	if (pair != ImageLibrary.end())
	{
		std::unique_ptr<FImage> storedImage = std::move(pair->second);
		ImageLibrary.erase(pair);
		ImageLibrary.insert({ GetUniqueName(InNewName), std::move(storedImage) });
	}
}

FImage* ImageManager::GetImage(std::string InName) const
{
	auto pair = ImageLibrary.find(InName);
	if (pair != ImageLibrary.end())
	{
		return pair->second.get();
	}
	else
	{
		return nullptr;
	}
}

std::pair<std::string, FImage*> ImageManager::CreateNewImage(std::string InDesiredName)
{
	auto newImage = make_unique<FImage>(256, 256);
	FImage* imagePtr = newImage.get();
	std::string assignedName = AddImage(std::move(newImage), InDesiredName);

	std::pair<std::string, FImage*> createdData = { assignedName, imagePtr };
	return createdData;
}

std::string ImageManager::AddImage(std::unique_ptr<FImage> InImage, std::string InDesiredName)
{
	std::string assignedName = GetUniqueName(InDesiredName);
	ImageLibrary.insert({ assignedName , std::move(InImage) });
	return assignedName;
}

void ImageManager::BeginImportImage()
{
	std::string imageFilename = UILibrary::PickFileName("Supported Files (*.jpg, *.png)\0*.jpg;*.png\0");
	if (imageFilename.size() > 0)
	{
		ImportImage(imageFilename);
	}
}

std::pair<std::string, class FImage*> ImageManager::ImportImage(std::string imageFilename)
{
	auto splitFilename = Split(imageFilename, '\\');
	std::string filename = Split(splitFilename[splitFilename.size() - 1], '.')[0];
	auto importedImage = FImage::LoadPNG(imageFilename, false);
	FImage* imagePtr = importedImage.get();
	std::string assignedName = AddImage(std::move(importedImage), filename);

	std::pair<std::string, FImage*> createdData = { assignedName, imagePtr };
	return createdData;
}

void ImageManager::SetRenderResult(std::unique_ptr<FImage> InImage)
{
	RenderResult->SetDimensions(InImage->GetWidth(), InImage->GetHeight());
	RenderResult->SetData(InImage->GetData());
}

std::string ImageManager::GetUniqueName(std::string InBaseName) const
{
	std::string baseName = InBaseName;
	std::string currentName = baseName;
	int count = 0;

	while (!IsNameUnique(currentName))
	{
		count++;
		currentName = fmt::format("{}.{:03}", baseName, count);
	}

	return currentName;
}

bool ImageManager::IsNameUnique(std::string InName) const
{
	return ImageLibrary.find(InName) == ImageLibrary.end();
}

}
