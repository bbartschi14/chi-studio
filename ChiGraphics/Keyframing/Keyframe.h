#pragma once
#include "ChiGraphics/External.h"
#include <vector>
#include "ChiGraphics/External.h"

namespace CHISTUDIO {

class IKeyframeBase
{
public:
	IKeyframeBase(int InFrame) : Frame(InFrame) {};

	// Custom UI per keyframe type. Returns true if UI was modified
	virtual bool RenderUI() { return false; };

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
