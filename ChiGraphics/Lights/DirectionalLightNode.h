#pragma once
#include "ChiGraphics/SceneNode.h"

namespace CHISTUDIO {

class DirectionalLightNode: public SceneNode
{
public:
	DirectionalLightNode(std::string InName);

private:
	glm::vec3 BaseDirection;

	/** Call to update the vertex object for debug visuals */
	void RefreshDebugVisual();
};

}
