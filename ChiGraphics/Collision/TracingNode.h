#pragma once
#include "ChiGraphics/SceneNode.h"

namespace CHISTUDIO {

class TracingNode: public SceneNode
{
public:
	TracingNode(std::string InName);

private:

	/** Call to update the vertex object for debug visuals */
	void RefreshDebugVisual();

	SceneNode* DebugSphereNode;
};

}
