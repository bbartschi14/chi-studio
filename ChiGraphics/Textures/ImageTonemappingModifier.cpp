#define NOMINMAX

#include "ImageTonemappingModifier.h"
#include "ChiGraphics/Textures/FImage.h"
#include "ChiCore/UI/UILibrary.h"
#include <functional>
#include <glm/gtx/compatibility.hpp>
#include "ChiGraphics/Utilities.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

namespace CHISTUDIO
{

const float A = 0.15f;
const float B = 0.50f;
const float C = 0.10f;
const float D = 0.20f;
const float E = 0.02f;
const float F = 0.30f;
const float W = 11.2f;

void ImageTonemappingModifier::ApplyModifier(FImage* InImageToModify) const
{
	for (int y = 0; y < InImageToModify->GetHeight(); y++)
	{
		for (int x = 0; x < InImageToModify->GetWidth(); x++)
		{
			glm::vec3 tonemappedResult = GetTonemappedValue(InImageToModify->GetPixel(x, y));
			InImageToModify->SetPixel(x, y, tonemappedResult);
		}
	}
}

bool ImageTonemappingModifier::RenderUI()
{
	bool wasModified = false;
	std::vector<std::string> typeStrings;
	typeStrings.push_back("Linear");
	typeStrings.push_back("Reinhard");
	typeStrings.push_back("HaarmPeterDuiker (UNIMPLEMENTED)");
	typeStrings.push_back("HejlBurgessDawson");
	typeStrings.push_back("Hable");

	int selectedMode = (int)TonemappingAlgorithm;
	if (selectedMode < typeStrings.size())
	{
		const char* currentTypeString = typeStrings[selectedMode].c_str();
		ImGui::SetNextItemWidth(205);
		if (ImGui::BeginCombo("##Algorithm", currentTypeString))
		{
			for (int i = 0; i < typeStrings.size(); i++)
			{
				if (i == 2) continue; // Skip unimplemented

				bool isSelected = currentTypeString == typeStrings[i];
				if (ImGui::Selectable(typeStrings[i].c_str(), isSelected))
				{
					currentTypeString = typeStrings[i].c_str();
					TonemappingAlgorithm = (ETonemappingAlgorithm)i;
					wasModified = true;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		ImGui::SameLine();
		ImGui::Text("Algorithm");
	}
	if (ImGui::SliderFloat("Exposure", &ExposureBias, 0.0f, 10.0f))
	{
		wasModified = true;
	}
	return wasModified;
}

glm::vec3 ImageTonemappingModifier::GetTonemappedValue(glm::vec3 InPixel) const
{
	InPixel = glm::pow(InPixel, glm::vec3(2.2f));
	switch (TonemappingAlgorithm)
	{
		case (ETonemappingAlgorithm::Linear):
		{
			InPixel *= ExposureBias;
			return glm::pow(InPixel, glm::vec3(1.0f / 2.2f));
		}
		case (ETonemappingAlgorithm::Reinhard):
		{
			InPixel *= ExposureBias;
			InPixel = InPixel / (glm::vec3(1.0f) + InPixel);
			return glm::pow(InPixel, glm::vec3(1.0f / 2.2f));
		}
		case (ETonemappingAlgorithm::HaarmPeterDuiker):
		{
			// Unimplemented
			std::cout << "Unimplemented tonemapping function" << std::endl;
			return glm::vec3(0.0f);
		}
		case (ETonemappingAlgorithm::HejlBurgessDawson):
		{
			InPixel *= ExposureBias;
			glm::vec3 x = glm::max(glm::vec3(0.0f), InPixel - glm::vec3(0.004f));
			return (x * (6.2f * x + .5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
		}
		case (ETonemappingAlgorithm::Hable):
		{
			const float baseExposureMultiplier = 3.0f;
			glm::vec3 current = Uncharted2Tonemap(ExposureBias * InPixel * baseExposureMultiplier);

			glm::vec3 whiteScale = 1.0f / Uncharted2Tonemap(glm::vec3(W));
			glm::vec3 color = current * whiteScale;

			return glm::pow(color, glm::vec3(1.0f / 2.2f));
		}
	}
	return glm::vec3(0.0f);
}

glm::vec3 ImageTonemappingModifier::Uncharted2Tonemap(glm::vec3 InColor) const
{
	return ((InColor * (A * InColor + C * B) + D * E) / (InColor * (A * InColor + B) + D * F)) - E / F;
}

}
