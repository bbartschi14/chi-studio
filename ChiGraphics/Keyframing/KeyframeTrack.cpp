#include "KeyframeTrack.h"
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/string_cast.hpp>

namespace CHISTUDIO {

	float BounceEaseOut(float InA, float InDelta, float InAlpha)
	{
		if (InAlpha < (1.0f / 2.75f))
			return InDelta * (7.5625f * InAlpha * InAlpha) + InA;
		else if (InAlpha < (2.0f / 2.75f))
			return InDelta * (7.5625f * (InAlpha -= (1.5f / 2.75f)) * InAlpha + .75f) + InA;
		else if (InAlpha < (2.5f / 2.75f))
			return InDelta * (7.5625f * (InAlpha -= (2.25f / 2.75f)) * InAlpha + .9375f) + InA;
		else
			return InDelta * (7.5625f * (InAlpha -= (2.625f / 2.75f)) * InAlpha + .984375f) + InA;
	}

	float BounceEaseIn(float InA, float InDelta, float InAlpha)
	{
		return InDelta - BounceEaseOut(0.0f, InDelta, 1.0f - InAlpha) + InA;
	}

	float BackEaseOut(float InA, float InDelta, float InAlpha)
	{
		InAlpha -= 1.0f;
		return InDelta * (InAlpha * InAlpha * ((1.70158f + 1) * InAlpha + 1.70158f) + 1) + InA;
	}

	float BackEaseIn(float InA, float InDelta, float InAlpha)
	{
		return InDelta * InAlpha * InAlpha * ((1.70158f + 1) * InAlpha - 1.70158f) + InA;
	}

	// http://gizma.com/easing/#quad3
	// https://gist.github.com/nitz/ae0a3775fd82df0d29658440fb322589
	float InterpolateValues(EKeyframeEasingType InEasingType, EKeyframeInterpolationMode InInterpolationMode, float InA, float InB, float InAlpha)
	{
		float delta = InB - InA;
		switch (InInterpolationMode)
		{
		case (EKeyframeInterpolationMode::Constant):
			return InA;

		case (EKeyframeInterpolationMode::Linear):
			return glm::lerp(InA, InB, InAlpha);

		case (EKeyframeInterpolationMode::Sinusoidal):
			switch (InEasingType)
			{
			case (EKeyframeEasingType::EaseIn):
				return -delta * glm::cos(InAlpha * (kPi / 2.0f)) + delta + InA;
			case (EKeyframeEasingType::EaseOut):
				return delta * glm::sin(InAlpha * (kPi / 2.0f)) + InA;
			case (EKeyframeEasingType::EaseInOut):
				return -delta / 2.0f * (glm::cos(kPi * InAlpha) - 1.0f) + InA;
			}
			break;

		case (EKeyframeInterpolationMode::Quadratic):
			switch (InEasingType)
			{
			case (EKeyframeEasingType::EaseIn):
				return delta * InAlpha * InAlpha + InA;
			case (EKeyframeEasingType::EaseOut):
				return -delta * InAlpha * (InAlpha - 2.0f) + InA;
			case (EKeyframeEasingType::EaseInOut):
				InAlpha *= 2.0f;
				if (InAlpha < 1.0f) 
					return delta / 2.0f * InAlpha * InAlpha + InA;
				InAlpha -= 1.0f;
				return -delta / 2.0f * (InAlpha * (InAlpha - 2.0f) - 1.0f) + InA;
			}
			break;

		case (EKeyframeInterpolationMode::Cubic):
			switch (InEasingType)
			{
			case (EKeyframeEasingType::EaseIn):
				return delta * InAlpha * InAlpha * InAlpha + InA;
			case (EKeyframeEasingType::EaseOut):
				InAlpha -= 1.0f;
				return delta * (InAlpha * InAlpha * InAlpha + 1) + InA;
			case (EKeyframeEasingType::EaseInOut):
				InAlpha *= 2.0f;
				if (InAlpha < 1.0f)
					return delta / 2.0f * InAlpha * InAlpha * InAlpha + InA;
				InAlpha -= 2.0f;
				return delta / 2.0f * (InAlpha * InAlpha * InAlpha + 2.0f) + InA;
			}
			break;

		case (EKeyframeInterpolationMode::Quartic):
			switch (InEasingType)
			{
			case (EKeyframeEasingType::EaseIn):
				return delta * InAlpha * InAlpha * InAlpha * InAlpha + InA;
			case (EKeyframeEasingType::EaseOut):
				InAlpha -= 1.0f;
				return -delta * (InAlpha * InAlpha * InAlpha * InAlpha - 1) + InA;
			case (EKeyframeEasingType::EaseInOut):
				InAlpha *= 2.0f;
				if (InAlpha < 1.0f)
					return delta / 2.0f * InAlpha * InAlpha * InAlpha * InAlpha + InA;
				InAlpha -= 2.0f;
				return -delta / 2.0f * (InAlpha * InAlpha * InAlpha * InAlpha - 2.0f) + InA;
			}
			break;

		case (EKeyframeInterpolationMode::Quintic):
			switch (InEasingType)
			{
			case (EKeyframeEasingType::EaseIn):
				return delta * InAlpha * InAlpha * InAlpha * InAlpha * InAlpha + InA;
			case (EKeyframeEasingType::EaseOut):
				InAlpha -= 1.0f;
				return delta * (InAlpha * InAlpha * InAlpha * InAlpha * InAlpha + 1) + InA;
			case (EKeyframeEasingType::EaseInOut):
				InAlpha *= 2.0f;
				if (InAlpha < 1.0f)
					return delta / 2.0f * InAlpha * InAlpha * InAlpha * InAlpha * InAlpha + InA;
				InAlpha -= 2.0f;
				return delta / 2.0f * (InAlpha * InAlpha * InAlpha * InAlpha * InAlpha + 2.0f) + InA;
			}
			break;

		case (EKeyframeInterpolationMode::Elastic):
			switch (InEasingType)
			{
				case (EKeyframeEasingType::EaseIn):
				{
					if (InAlpha == 1.0f)
						return delta + InA;

					float p = 0.3f;
					float s = p / 4.0f;

					return -(delta * glm::pow(2, 10 * (InAlpha -= 1.0f)) * glm::sin((InAlpha - s) * (2.0f * kPi) / p)) + InA;
				}
				case (EKeyframeEasingType::EaseOut):
				{
					if (InAlpha == 1.0f)
						return delta + InA;

					float p1 = 0.3f;
					float s1 = p1 / 4.0f;

					return delta * glm::pow(2, -10 * (InAlpha)) * glm::sin((InAlpha - s1) * (2.0f * kPi) / p1) + delta + InA;
				}
				case (EKeyframeEasingType::EaseInOut):
				{
					if ((InAlpha *= 2.0f) == 2.0f)
						return delta + InA;

					float p2 = 0.3f * 1.5f;
					float s2 = p2 / 4.0f;

					if (InAlpha < 1.0f)
					{
						return -0.5f * (delta * glm::pow(2, 10 * (InAlpha -= 1.0f)) * glm::sin((InAlpha - s2) * (2.0f * kPi) / p2)) + InA;
					}
					else
					{
						return delta * glm::pow(2, -10 * (InAlpha -= 1.0f)) * glm::sin((InAlpha - s2) * (2.0f * kPi) / p2) * 0.5f + delta + InA;
					}
				}
			}
			break;

		case (EKeyframeInterpolationMode::Bounce):
			switch (InEasingType)
			{
				case (EKeyframeEasingType::EaseIn):
				{
					return BounceEaseIn(InA, delta, InAlpha);
				}
				case (EKeyframeEasingType::EaseOut):
				{
					return BounceEaseOut(InA, delta, InAlpha);
				}
				case (EKeyframeEasingType::EaseInOut):
				{
					if (InAlpha < 0.5f)
						return BounceEaseIn(0.0f, delta, InAlpha * 2.0f) * 0.5f + InA;
					else
						return BounceEaseOut(0.0f, delta, InAlpha * 2.0f - 1.0f) * .5f + delta * .5f + InA;
				}
			}
			break;

		case (EKeyframeInterpolationMode::Back):
			switch (InEasingType)
			{
			case (EKeyframeEasingType::EaseIn):
			{
				return BackEaseIn(InA, delta, InAlpha);
			}
			case (EKeyframeEasingType::EaseOut):
			{
				return BackEaseOut(InA, delta, InAlpha);
			}
			case (EKeyframeEasingType::EaseInOut):
			{
				float s = 1.70158f;
				if ((InAlpha *= 2.0f) < 1.0f)
					return delta / 2.0f * (InAlpha * InAlpha * (((s *= (1.525f)) + 1) * InAlpha - s)) + InA;
				return delta / 2.0f * ((InAlpha -= 2.0f) * InAlpha * (((s *= (1.525f)) + 1) * InAlpha + s) + 2) + InA;
			}
			}
			break;
		}


		return InA;
	}

	template <>
	glm::vec3 FKeyframeTrack<glm::vec3>::GetValueAtFrame(int InFrame)
	{		
		int keyIndexOne, keyIndexTwo;
		EKeyframeEvaluationType evalType = EvaluateFrame(InFrame, &keyIndexOne, &keyIndexTwo);
		switch (evalType)
		{
		case EKeyframeEvaluationType::IsExact:
			return Keyframes[keyIndexOne]->Value;
		case EKeyframeEvaluationType::HasTwoNeighbors:
		{
			float alpha = (InFrame - Keyframes[keyIndexOne]->Frame) / (float)(Keyframes[keyIndexTwo]->Frame - Keyframes[keyIndexOne]->Frame);
			//std::cout << "Between keyframes: " << glm::to_string(glm::lerp(Keyframes[keyIndexOne]->Value, Keyframes[keyIndexTwo]->Value, alpha)) << std::endl;\

			glm::vec3 firstVal = Keyframes[keyIndexOne]->Value;
			glm::vec3 secondVal = Keyframes[keyIndexTwo]->Value;
			float interpedX = InterpolateValues(Keyframes[keyIndexOne]->EasingType, Keyframes[keyIndexOne]->InterpolationMode, firstVal.x, secondVal.x, alpha);
			float interpedY = InterpolateValues(Keyframes[keyIndexOne]->EasingType, Keyframes[keyIndexOne]->InterpolationMode, firstVal.y, secondVal.y, alpha);
			float interpedZ = InterpolateValues(Keyframes[keyIndexOne]->EasingType, Keyframes[keyIndexOne]->InterpolationMode, firstVal.z, secondVal.z, alpha);
			return glm::vec3(interpedX, interpedY, interpedZ);
		}
		case EKeyframeEvaluationType::HasNeighborBefore:
			return Keyframes[keyIndexOne]->Value;
		case EKeyframeEvaluationType::HasNeighborAfter:
			return Keyframes[keyIndexTwo]->Value;
		default:
			return glm::vec3(0.0f);
		}
	

		return glm::vec3(0.0f);
	}

	template <>
	float FKeyframeTrack<float>::GetValueAtFrame(int InFrame)
	{
		int keyIndexOne, keyIndexTwo;
		EKeyframeEvaluationType evalType = EvaluateFrame(InFrame, &keyIndexOne, &keyIndexTwo);
		switch (evalType)
		{
		case EKeyframeEvaluationType::IsExact:
			return Keyframes[keyIndexOne]->Value;
		case EKeyframeEvaluationType::HasTwoNeighbors:
		{
			float alpha = (InFrame - Keyframes[keyIndexOne]->Frame) / (float)(Keyframes[keyIndexTwo]->Frame - Keyframes[keyIndexOne]->Frame);
			float firstVal = Keyframes[keyIndexOne]->Value;
			float secondVal = Keyframes[keyIndexTwo]->Value;
			float interpedX = InterpolateValues(Keyframes[keyIndexOne]->EasingType, Keyframes[keyIndexOne]->InterpolationMode, firstVal, secondVal, alpha);
			return interpedX;
		}
		case EKeyframeEvaluationType::HasNeighborBefore:
			return Keyframes[keyIndexOne]->Value;
		case EKeyframeEvaluationType::HasNeighborAfter:
			return Keyframes[keyIndexTwo]->Value;
		default:
			return (0.0f);
		}


		return (0.0f);
	}
}