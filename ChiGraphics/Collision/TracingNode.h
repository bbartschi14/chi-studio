#pragma once
#include "ChiGraphics/SceneNode.h"

namespace CHISTUDIO {

/* Represents a Ray-Traceable sphere. Displayed in the scene viewport using a debug sphere mesh,
 * but is traced using exact ray-sphere intersection.
 */
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
