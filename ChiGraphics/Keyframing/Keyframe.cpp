#include "Keyframe.h"
#include <iostream>
#include "ChiCore/UI/UILibrary.h"

namespace CHISTUDIO {

template<class T>
bool FKeyframe<T>::RenderUI()
{
	return false;
}

template<>
bool FKeyframe<glm::vec3>::RenderUI()
{
	return UILibrary::DrawVector3ControlVertical("Key Value", Value, 0.1f);
}

template<>
bool FKeyframe<float>::RenderUI()
{
	return ImGui::DragFloat("Key Value", &Value, 1.0f);
}

bool IKeyframeBase::RenderBaseUI()
{
	ImGui::Text(fmt::format("Frame: {}", Frame).c_str());

	std::vector<std::string> interpModeStrings{"Constant", "Linear", "Sinusoidal", "Quadratic", "Cubic", "Quartic", "Quintic", "Elastic", "Bounce", "Back"};
	int selectedInterpMode = (int)InterpolationMode;
	const char* currentInterpModeString = interpModeStrings[selectedInterpMode].c_str();
	ImGui::Text("Interpolation");
	//ImGui::SetNextItemWidth(200);
	if (ImGui::BeginCombo("##InterpMode", currentInterpModeString))
	{
		for (int i = 0; i < interpModeStrings.size(); i++)
		{
			bool isSelected = currentInterpModeString == interpModeStrings[i];
			if (ImGui::Selectable(interpModeStrings[i].c_str(), isSelected))
			{
				currentInterpModeString = interpModeStrings[i].c_str();
				InterpolationMode = (EKeyframeInterpolationMode)i;
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	std::vector<std::string> easingTypeStrings{ "Ease In", "Ease Out", "Ease In Out" };
	int selectedEasingType = (int)EasingType;
	const char* currentEasingTypeString = easingTypeStrings[selectedEasingType].c_str();
	ImGui::Text("Easing");
	//ImGui::SetNextItemWidth(200);
	if (ImGui::BeginCombo("##EasingType", currentEasingTypeString))
	{
		for (int i = 0; i < easingTypeStrings.size(); i++)
		{
			bool isSelected = currentEasingTypeString == easingTypeStrings[i];
			if (ImGui::Selectable(easingTypeStrings[i].c_str(), isSelected))
			{
				currentEasingTypeString = easingTypeStrings[i].c_str();
				EasingType = (EKeyframeEasingType)i;
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
	
	ImGui::Separator();

	return RenderUI();
}

}