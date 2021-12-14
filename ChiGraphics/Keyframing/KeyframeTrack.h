#pragma once
#include "Keyframe.h"
#include <vector>
#include <memory>
#include <typeinfo>
#include <stdexcept>
#include "ChiGraphics/Utilities.h"
#include <iostream>
#include <cmath>

namespace CHISTUDIO {

template <class T>
class FKeyframeTrack
{
public:
	FKeyframeTrack() {}
	~FKeyframeTrack() {}

	inline void AddKeyframe(int InFrame, T InValue)
	{
		std::shared_ptr<FKeyframe<T>> key = std::make_shared<FKeyframe<T>>(InValue, InFrame);
		Keyframes.push_back(std::move(key));
	}

	T GetValueAtFrame(int InFrame);

	// Given the input frame, find the keyframe nearby keyframes. If there is a keyframe at the frame, return true and return the keyframe in the first output
	bool GetNeighborKeyframes(int InFrame, int* OutKeyframeOne, int* OutKeyframeTwo, bool* OutFoundPrevious, bool* OutFoundNext)
	{
		*OutKeyframeOne = std::numeric_limits<int>::min();
		*OutKeyframeTwo = std::numeric_limits<int>::max();
		*OutFoundPrevious = false;
		*OutFoundNext = false;

		size_t numKeyframes = Keyframes.size();
		for (size_t i = 0; i < numKeyframes; i++)
		{
			if (Keyframes[i]->Frame == InFrame)
			{
				*OutKeyframeOne = (int)i;
				return true;
			}
			else if (Keyframes[i]->Frame < InFrame)
			{
				if (!*OutFoundPrevious || Keyframes[i]->Frame > Keyframes[*OutKeyframeOne]->Frame )
				{
					*OutKeyframeOne = (int)i;
					*OutFoundPrevious = true;
				}
			}
			else if (Keyframes[i]->Frame > InFrame)
			{
				if (!*OutFoundNext || Keyframes[i]->Frame < Keyframes[*OutKeyframeTwo]->Frame)
				{
					*OutKeyframeTwo = (int)i;
					*OutFoundNext = true;
				}
			}
		}

		return false;
	}

	bool HasKeyframes() const { return Keyframes.size(); }

	inline std::vector<IKeyframeBase*> FKeyframeTrack<T>::GetKeyframes()
	{
		std::vector<IKeyframeBase*> keyframesToReturn;
		for (size_t i = 0; i < Keyframes.size(); i++)
		{
			keyframesToReturn.push_back(Keyframes[i].get());
		}
		return keyframesToReturn;
	}
private:
	std::vector<std::shared_ptr<FKeyframe<T>>> Keyframes;

};

}
