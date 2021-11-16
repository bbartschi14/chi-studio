#include "WASDCameraNode.h"
#include "ChiGraphics/Components/CameraComponent.h"
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include "ChiGraphics/InputManager.h"

namespace CHISTUDIO {

WASDCameraNode::WASDCameraNode(std::string InName, float fov, float aspect, float speed)
	: SceneNode(InName), Speed(speed)
{
	auto camera = make_unique<CameraComponent>(fov, aspect, 0.1f, 100.f);
	AddComponent(std::move(camera));
}

void WASDCameraNode::Update(double InDeltaTime)
{
	float delta_dist = Speed * static_cast<float>(InDeltaTime);
	glm::vec3 old_position = GetTransform().GetPosition();
	glm::vec3 new_position = old_position;
	if (InputManager::GetInstance().IsKeyPressed('W')) {
		new_position += delta_dist * GetTransform().GetForwardDirection();
	}
	if (InputManager::GetInstance().IsKeyPressed('S')) {
		new_position -= delta_dist * GetTransform().GetForwardDirection();
	}
	if (InputManager::GetInstance().IsKeyPressed('A')) {
		new_position -= delta_dist * GetTransform().GetRightDirection();
	}
	if (InputManager::GetInstance().IsKeyPressed('D')) {
		new_position += delta_dist * GetTransform().GetRightDirection();
	}
	GetTransform().SetPosition(new_position);
}

}