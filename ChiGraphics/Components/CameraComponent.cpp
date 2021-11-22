#include "CameraComponent.h"

#include "ChiGraphics/Utilities.h"
#include "ChiGraphics/SceneNode.h"
#include "ChiGraphics/Collision/FRay.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace CHISTUDIO {

CameraComponent::CameraComponent(float InFOV, float InAspectRatio, float InZNear, float InZFar, float InFocusDistance, float InAperture)
	: FOV(InFOV), AspectRatio(InAspectRatio), ZNear(InZNear), ZFar(InZFar), ViewMatrix(nullptr), FocusDistance(InFocusDistance), Aperture(InAperture)
{
}

glm::mat4 CameraComponent::GetProjectionMatrix() const
{
	return glm::perspective(FOV * kPi / 180.f, AspectRatio, ZNear, ZFar);
}

glm::mat4 CameraComponent::GetViewMatrix() const
{
	if (ViewMatrix == nullptr) {
		return glm::inverse(GetNodePtr()->GetTransform().GetLocalToWorldMatrix());
	}
	else {
		return *ViewMatrix;
	}
}

glm::vec2 CameraComponent::GetNormalizedDeviceCoords(glm::vec2 InMousePosition, glm::vec2 InViewportSize)
{
	float x = (2.0f * InMousePosition.x) / InViewportSize.x - 1.0f;
	float y = (2.0f * InMousePosition.y) / InViewportSize.y - 1.0f;

	return glm::vec2(x, -y);
}

glm::vec4 CameraComponent::GetEyeCoords(glm::vec4 InClipCoords) 
{
	glm::mat4 invertedProjection = glm::inverse(GetProjectionMatrix());
	return invertedProjection * InClipCoords;
}

glm::vec3 CameraComponent::GetWorldCoords(glm::vec4 eyeCoords)
{
	glm::mat4 invertedView = glm::inverse(GetViewMatrix());
	glm::vec4 dividedEye = glm::vec4(eyeCoords[0] / eyeCoords[3], eyeCoords[1] / eyeCoords[3], eyeCoords[2] / eyeCoords[3], 0);
	glm::vec4 worldRay = invertedView * dividedEye;
	glm::vec3 mouseRay = glm::vec3(worldRay[0], worldRay[1], worldRay[2]);
	return mouseRay;
}

FRay CameraComponent::GenerateRay(glm::vec2 InMousePosition, glm::vec2 InViewportSize)
{
	glm::vec2 normalizedCoords = GetNormalizedDeviceCoords(InMousePosition, InViewportSize);
	glm::vec4 clipCoords = glm::vec4(normalizedCoords[0], normalizedCoords[1], -1.0f, 1.0f);
	glm::vec4 eyeCoords = GetEyeCoords(clipCoords);

	glm::vec3 worldRay = glm::normalize(GetWorldCoords(eyeCoords));

	return FRay(GetAbsoluteCameraPosition(), worldRay);
}

}
