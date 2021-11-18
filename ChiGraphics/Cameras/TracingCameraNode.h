#pragma once

#include "ChiGraphics/SceneNode.h"

namespace CHISTUDIO {

/** Camera node that can be used for ray trace rendering */
class TracingCameraNode: public SceneNode
{
public:
	TracingCameraNode(std::string InName, float InFOV = 45.f, float InAspectRatio = 0.75f);
	std::unique_ptr<class FTracingCamera> GetTracingCamera(glm::ivec2 InRenderSize) const;

private:

	/** Call to update the vertex object for debug visuals */
	void RefreshDebugVisual();

};

}
