#include "KeyframeTrack.h"
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/string_cast.hpp>

namespace CHISTUDIO {

	template <>
	glm::vec3 FKeyframeTrack<glm::vec3>::GetValueAtFrame(int InFrame)
	{		
		int keyIndexOne, keyIndexTwo;
		bool foundOne, foundTwo;
		bool isExact = GetNeighborKeyframes(InFrame, &keyIndexOne, &keyIndexTwo, &foundOne, &foundTwo);
		//std::cout << "Found One: " << foundOne << ", " << "Found Two: " << foundTwo << ", " << "keyIndexOne: " << keyIndexOne << ", " << "keyIndexTwo: " << keyIndexTwo << std::endl;
		if (isExact)
		{
			return Keyframes[keyIndexOne]->Value;
		}
		else
		{
			if (foundOne && foundTwo)
			{
				float alpha = (InFrame - Keyframes[keyIndexOne]->Frame) / (float)(Keyframes[keyIndexTwo]->Frame - Keyframes[keyIndexOne]->Frame);
				//std::cout << "Between keyframes: " << glm::to_string(glm::lerp(Keyframes[keyIndexOne]->Value, Keyframes[keyIndexTwo]->Value, alpha)) << std::endl;
				return glm::lerp(Keyframes[keyIndexOne]->Value, Keyframes[keyIndexTwo]->Value, alpha);
			}
			else if (foundOne)
			{
				//std::cout << "In front of keyframe: " << glm::to_string(Keyframes[keyIndexOne]->Value) << std::endl;
				return Keyframes[keyIndexOne]->Value;
			}
			else if (foundTwo)
			{
				//std::cout << "Before keyframe: " << glm::to_string(Keyframes[keyIndexTwo]->Value) << std::endl;
				return Keyframes[keyIndexTwo]->Value;
			}
		}

		return glm::vec3(0.0f);
	}
}