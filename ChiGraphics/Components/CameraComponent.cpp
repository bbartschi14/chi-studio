#include "CameraComponent.h"

#include "ChiGraphics/Utilities.h"
#include "ChiGraphics/SceneNode.h"
#include "ChiGraphics/Collision/FRay.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "ChiGraphics/Cameras/ArcBallCameraNode.h"
#include "ChiGraphics/Cameras/TracingCameraNode.h"

namespace CHISTUDIO {

CameraComponent::CameraComponent(float InFOV, float InAspectRatio, float InZNear, float InZFar, float InFocusDistance, float InAperture)
	: IKeyframeable("Camera Component"), FOV(InFOV), AspectRatio(InAspectRatio), ZNear(InZNear), ZFar(InZFar), ViewMatrix(nullptr), FocusDistance(InFocusDistance), Aperture(InAperture), bIsPerspective(true)
{
}

glm::mat4 CameraComponent::GetProjectionMatrix() const
{
	if (bIsPerspective)
	{
		return glm::perspective(FOV * kPi / 180.f, AspectRatio, ZNear, ZFar);
	}
	else
	{
		float orthoHeight = orthoWidth / AspectRatio;
		return glm::ortho(-(orthoWidth / 2.0f), orthoWidth / 2.0f,
			-orthoHeight / 2.0f, (orthoHeight / 2.0f),
			-1000.0f, 1000.0f);
	}
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

void CameraComponent::SetFocusDistance(float InFocusDistance)
{
	FocusDistance = InFocusDistance;
	TracingCameraNode* tracingCameraNode = dynamic_cast<TracingCameraNode*>(GetNodePtr());
	if (tracingCameraNode)
	{
		tracingCameraNode->RefreshDebugVisual();
	}
}

void CameraComponent::ApplyKeyframeData(int InFrame)
{
	if (FOVKeyframeTrack.HasKeyframes())
	{
		SetFOV(FOVKeyframeTrack.GetValueAtFrame(InFrame));
	}

	if (FocusDistanceKeyframeTrack.HasKeyframes())
	{
		SetFocusDistance(FocusDistanceKeyframeTrack.GetValueAtFrame(InFrame));
	}

	if (ApertureKeyframeTrack.HasKeyframes())
	{
		Aperture = (ApertureKeyframeTrack.GetValueAtFrame(InFrame));
	}
}

std::vector<std::string> CameraComponent::GetKeyframeTrackNames() const
{
	std::vector<std::string> names = { "FOV", "Focus Distance", "Aperture"};
	return names;
}

void CameraComponent::CreateKeyframeOnTrack(std::string InTrackName, int InFrame)
{
	if (InTrackName == "FOV")
	{
		FOVKeyframeTrack.AddKeyframe(InFrame, FOV);
	}
	else if (InTrackName == "Focus Distance")
	{
		FocusDistanceKeyframeTrack.AddKeyframe(InFrame, FocusDistance);
	}
	else if (InTrackName == "Aperture")
	{
		ApertureKeyframeTrack.AddKeyframe(InFrame, Aperture);
	}
}

void CameraComponent::DeleteKeyframeOnTrack(std::string InTrackName, int InIndex)
{
	if (InTrackName == "FOV")
	{
		FOVKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
	}
	else if (InTrackName == "Focus Distance")
	{
		FocusDistanceKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
	}
	else if (InTrackName == "Aperture")
	{
		ApertureKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
	}
}

std::vector<IKeyframeBase*> CameraComponent::GetKeyframesOnTrack(std::string InTrackName)
{
	if (InTrackName == "FOV")
	{
		return FOVKeyframeTrack.GetKeyframes();
	}
	else if (InTrackName == "Focus Distance")
	{
		return FocusDistanceKeyframeTrack.GetKeyframes();
	}
	else if (InTrackName == "Aperture")
	{
		return ApertureKeyframeTrack.GetKeyframes();
	}

	return std::vector<IKeyframeBase*>();
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

	FRay ray = FRay(glm::vec3(0.0f), glm::vec3(0.0f));
	if (bIsPerspective)
	{
		ray =  FRay(GetAbsoluteCameraPosition(), worldRay);
	}
	else
	{
		glm::vec3 cameraRight = glm::normalize(glm::cross(-worldRay, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, -worldRay));
		std::cout << "Camera Up: " << glm::to_string(cameraUp) << " Camera Right: " << glm::to_string(cameraRight) << std::endl;

		glm::vec3 origin = GetAbsoluteCameraPosition();// + (cameraUp * normalizedCoords.y * orthoWidth / AspectRatio / 2.0f) + (cameraRight * normalizedCoords.x * orthoWidth / 2.0f);
		//std::cout << glm::to_string(-worldRay) << std::endl;
		//std::cout << glm::to_string(normalizedCoords) << std::endl;
		ray = FRay(origin, -worldRay);
	}
	return ray;
}

}
