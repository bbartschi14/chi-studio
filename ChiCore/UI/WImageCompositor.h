#pragma once

#include "IWidget.h"
#include <vector>
#include <memory>

namespace CHISTUDIO {

class WImageCompositor : public IWidget
{
public:
	WImageCompositor();

	void Render(Application& InApplication, float InDeltaTime) override;
	void SelectImagePtr(class FImage* InImagePtr);

	// Add a modifier to the list of image modifiers
	void AddModifier(std::unique_ptr<class IImageModifier> InModifier, bool ShouldRecalculate = true);

	// Remove at an index
	void RemoveModifier(int InIndex);

	//
	void RecalculateModifiers();

	void ApplyModifiersToImage(class FImage* InImagePtr);

	size_t GetNumberOfModifiers() const 
	{ 
		return ImageModifiers.size(); 
	}

	const std::vector<std::unique_ptr<class IImageModifier>>& GetModifiers() const
	{
		return ImageModifiers;
	}

private:
	float ImageZoom;
	class FImage* SelectedImagePtr;
	std::unique_ptr<class FImage> ModifiedImagePtr;
	std::unique_ptr<class FTexture> DisplayTexture;
	std::vector < std::unique_ptr < class IImageModifier >> ImageModifiers;
};

}

