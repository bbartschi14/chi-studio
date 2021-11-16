#pragma once
#include "ChiGraphics/SceneNode.h"

namespace CHISTUDIO {

class CameraComponent;

class WASDCameraNode : public SceneNode {
public:
	WASDCameraNode(std::string InName, float fov = 45.f, float aspect = 0.75f, float speed = 2.0f);
	void Update(double InDeltaTime) override;

private:

	float Speed;
};

}