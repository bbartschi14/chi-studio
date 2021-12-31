#include "WImageCompositor.h"
#include "ChiGraphics/Textures/ImageManager.h"
#include "UILibrary.h"
#include <memory>
#include <map>
#include <string>
#include "ChiGraphics/GL_Wrapper/FTexture.h"
#include "ChiGraphics/Utilities.h"
#include "ChiGraphics/Textures/ImageModifier.h"
#include "ChiGraphics/Textures/ImageBlurModifier.h"
#include "ChiGraphics/Textures/ImageThresholdModifier.h"
#include "ChiGraphics/Textures/ImageBloomModifier.h"
#include "ChiGraphics/Textures/ImageTonemappingModifier.h"

namespace CHISTUDIO {

WImageCompositor::WImageCompositor()
{
	SelectedImagePtr = nullptr;
	ImageZoom = 1.0f;
	DisplayTexture = nullptr;
	ModifiedImagePtr = make_unique<FImage>(64, 64);
}

void WImageCompositor::Render(Application& InApplication, float InDeltaTime)
{
	ImGui::Begin("Image Compositor");

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
	ImGui::BeginChild("ImageLibrary", ImVec2{ 300, 100 }, true, window_flags);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Images"))
		{
			//if (ImGui::MenuItem("Create Material")) { MaterialManager::GetInstance().CreateNewMaterial(); }
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	std::map<std::string, FImage*> selectableImages;
	selectableImages.insert({ "Render Result", ImageManager::GetInstance().GetRenderResult()});

	for (auto imagePair : selectableImages)
	{
		if (ImGui::Selectable(imagePair.first.c_str(), SelectedImagePtr == imagePair.second))
		{
			SelectImagePtr(imagePair.second);
		}
	}

	ImGui::EndChild();

	ImGui::SetNextItemWidth(300.0f);
	ImGui::SliderFloat("Image Zoom", &ImageZoom, 0.1f, 10.0f);

	const float modifierWidth = 340.0f;
	ImGui::BeginChild("RenderResult", ImVec2(ImGui::GetContentRegionAvail().x - modifierWidth, ImGui::GetContentRegionAvail().y), true, window_flags | ImGuiWindowFlags_HorizontalScrollbar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Result"))
		{
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (DisplayTexture)
	{
		uint64_t textureID = DisplayTexture->GetHandle();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ (float)SelectedImagePtr->GetWidth() * ImageZoom, (float)SelectedImagePtr->GetHeight() * ImageZoom }, ImVec2{ 0, 0 }, ImVec2{ 1, 1 });
	}
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("ImageModifiers", ImVec2(modifierWidth - 6.0f, ImGui::GetContentRegionAvail().y), true, window_flags | ImGuiWindowFlags_HorizontalScrollbar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Add Modifier"))
		{
			if (ImGui::MenuItem("Blur")) 
			{ 			
				auto blurMod = make_unique<ImageBlurModifier>(5);
				AddModifier(std::move(blurMod));
			}
			if (ImGui::MenuItem("Threshold"))
			{
				auto thresholdMod = make_unique<ImageThresholdModifier>(0.5f);
				AddModifier(std::move(thresholdMod));
			}
			if (ImGui::MenuItem("Bloom"))
			{
				auto bloomMod = make_unique<ImageBloomModifier>(1.0f, 20);
				AddModifier(std::move(bloomMod));
			}
			if (ImGui::MenuItem("Tonemapping"))
			{
				auto tonemappingMod = make_unique<ImageTonemappingModifier>(ETonemappingAlgorithm::Hable, 1.0f);
				AddModifier(std::move(tonemappingMod));
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	bool wasModified = false;
	int indexToRemove = -1;
	for (size_t i = 0; i < ImageModifiers.size(); i++)
	{
		wasModified |= UILibrary::RenderImageModifier(ImageModifiers[i].get(), (int)i, indexToRemove);
	}
	if (indexToRemove > -1)
	{
		RemoveModifier(indexToRemove);
	}
	if (wasModified)
	{
		RecalculateModifiers();
	}

	ImGui::EndChild();

	ImGui::End();
}

void WImageCompositor::SelectImagePtr(FImage* InImagePtr)
{
	if (InImagePtr)
	{
		SelectedImagePtr = InImagePtr;
		RecalculateModifiers();
	}
}

void WImageCompositor::AddModifier(std::unique_ptr<class IImageModifier> InModifier, bool ShouldRecalculate)
{
	ImageModifiers.push_back(std::move(InModifier));
	if (ShouldRecalculate) RecalculateModifiers();
}

void WImageCompositor::RemoveModifier(int InIndex)
{
	if (InIndex < ImageModifiers.size())
	{
		ImageModifiers.erase(ImageModifiers.begin() + InIndex);
	}
}

void WImageCompositor::RecalculateModifiers()
{
	if (!SelectedImagePtr) return;

	// 1. Copy selected image to a blank image
	ModifiedImagePtr = FImage::MakeImageCopy(SelectedImagePtr);

	// 2. Apply modifiers
	ApplyModifiersToImage(ModifiedImagePtr.get());

	DisplayTexture = make_unique<FTexture>();
	DisplayTexture->UpdateImage(*ModifiedImagePtr);
}

void WImageCompositor::ApplyModifiersToImage(FImage* InImagePtr)
{
	for (size_t i = 0; i < ImageModifiers.size(); i++)
	{
		ImageModifiers[i]->ApplyModifier(InImagePtr);
	}
}

}