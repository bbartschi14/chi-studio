#pragma once
#include "ChiGraphics/SceneNode.h"

namespace CHISTUDIO {

class LightNode: public SceneNode
{
public:
	LightNode(std::string InName);

private:

	/** Call to update the vertex object for debug visuals */
	void RefreshDebugVisual();
};

}
