#pragma once

#include "ChiGraphics/SceneNode.h"

namespace CHISTUDIO {

/** Camera node that can be used for ray trace rendering */
class TracingCameraNode: public SceneNode
{
public:
	TracingCameraNode(std::string InName, float InFOV = 30.f, float InAspectRatio = 0.75f);
	std::unique_ptr<class FTracingCamera> GetTracingCamera(glm::ivec2 InRenderSize) const;

	/** Call to update the vertex object for debug visuals */
	void RefreshDebugVisual();
private:

	std::unique_ptr<FPositionArray> GetDebugPositions();

	std::unique_ptr<SceneNode> CameraDebugVisualNode;
	class CameraComponent* Camera;
	class VertexObject* DebugMesh;
};

}
