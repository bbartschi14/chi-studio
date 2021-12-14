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

}