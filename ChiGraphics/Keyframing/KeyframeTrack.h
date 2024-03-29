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

enum class EKeyframeEvaluationType
{
	IsExact,
	HasTwoNeighbors,
	HasNeighborBefore,
	HasNeighborAfter,
	HasNone
};


class IKeyframeTrackBase
{
public:
	IKeyframeTrackBase() {};
};

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

	void DeleteKeyframeAtIndex(int InIndex)
	{
		if (InIndex >= 0 && InIndex < Keyframes.size())
			Keyframes.erase(Keyframes.begin() + InIndex);
	}

	T GetValueAtFrame(int InFrame);

	EKeyframeEvaluationType EvaluateFrame(int InFrame, int* OutKeyframeOne, int* OutKeyframeTwo)
	{
		int keyIndexOne, keyIndexTwo;
		bool foundOne, foundTwo;
		bool isExact = GetNeighborKeyframes(InFrame, &keyIndexOne, &keyIndexTwo, &foundOne, &foundTwo);
		if (isExact)
		{
			*OutKeyframeOne = keyIndexOne;
			return EKeyframeEvaluationType::IsExact;
		}
		else
		{
			if (foundOne && foundTwo)
			{
				*OutKeyframeOne = keyIndexOne;
				*OutKeyframeTwo = keyIndexTwo;
				return EKeyframeEvaluationType::HasTwoNeighbors;
			}
			else if (foundOne)
			{
				*OutKeyframeOne = keyIndexOne;
				return EKeyframeEvaluationType::HasNeighborBefore;
			}
			else if (foundTwo)
			{
				*OutKeyframeTwo = keyIndexTwo;
				return EKeyframeEvaluationType::HasNeighborAfter;
			}
		}

		return EKeyframeEvaluationType::HasNone;
	}

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

	std::vector<IKeyframeBase*> GetKeyframes()
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
