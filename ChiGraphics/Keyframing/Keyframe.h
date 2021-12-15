#pragma once
#include "ChiGraphics/External.h"
#include <vector>
#include "ChiGraphics/External.h"

namespace CHISTUDIO {

enum class EKeyframeInterpolationMode
{
	Constant,
	Linear,
	Sinusoidal,
	Quadratic,
	Cubic,
	Quartic,
	Quintic,
	Elastic,
	Bounce,
	Back
};

enum class EKeyframeEasingType
{
	EaseIn,
	EaseOut,
	EaseInOut
};

class IKeyframeBase
{
public:
	IKeyframeBase(int InFrame) 
		: EasingType(EKeyframeEasingType::EaseInOut), 
		InterpolationMode(EKeyframeInterpolationMode::Quadratic), 
		Frame(InFrame) 
	{};

	// Custom UI per keyframe type. Returns true if UI was modified
	virtual bool RenderUI() { return false; };
	bool RenderBaseUI();
	EKeyframeEasingType EasingType;
	EKeyframeInterpolationMode InterpolationMode;
	int Frame;
};

template <class T>
class FKeyframe : public IKeyframeBase
{
public:
	FKeyframe(T InValue, int InFrame) : IKeyframeBase(InFrame), Value(InValue) {};

	// Custom UI per keyframe type. Returns true if UI was modified
	bool RenderUI() override;

	T Value;
};

}
