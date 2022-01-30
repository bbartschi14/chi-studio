#pragma once

#include "ChiGraphics/SceneNode.h"

namespace CHISTUDIO {

/** Camera node that can be used for ray-traced rendering. Is not used for 
 *  rendering the scene viewport. Focus distance and FOV displayed with debug
 *  line meshes in the scene.
 */
class TracingCameraNode: public SceneNode
{
public:
	TracingCameraNode(std::string InName, float InFOV = 30.f, float InAspectRatio = 0.75f);
	std::unique_ptr<class FTracingCamera> GetTracingCamera(glm::ivec2 InRenderSize) const;

	/** Call to update the vertex object for debug visuals */
	void RefreshDebugVisual();

private:
	/** Use the camera properties to calculate the debug visual */
	std::unique_ptr<FPositionArray> GetDebugPositions();

	std::unique_ptr<SceneNode> CameraDebugVisualNode;
	class CameraComponent* Camera;
	class VertexObject* DebugMesh;
};

}
