#pragma once
#include "ChiGraphics/SceneNode.h"

namespace CHISTUDIO {

class AmbientLightNode: public SceneNode
{
public:
	AmbientLightNode(std::string InName);

private:

	/** Call to update the vertex object for debug visuals */
	void RefreshDebugVisual();
};

}
