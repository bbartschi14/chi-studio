#include "Transform.h"

#include <memory>
#include <stdexcept>
#include <glm/gtx/matrix_decompose.hpp>
#include "ChiGraphics/Utilities.h"
#include "ChiGraphics/SceneNode.h"
#include <glm/gtx/string_cast.hpp>

namespace CHISTUDIO {

Transform::Transform(SceneNode& InNode)
	: Position(0.f), EulerRotation(glm::vec3(0.f, 0.f, 0.f)), Scale(glm::vec3(1.f)), Node(InNode)
{
	UNUSED(Node);
	UpdateLocalTransformMatrix();


}

Transform::~Transform()
{
}

void Transform::SetPosition(const glm::vec3& InPosition)
{
	Position = InPosition;
	UpdateLocalTransformMatrix();
}

void Transform::SetRotation(const glm::quat& InRotation)
{
	EulerRotation = glm::degrees(glm::eulerAngles(InRotation));
	UpdateLocalTransformMatrix();
}

void Transform::SetRotation(const glm::vec3& InAxis, float InAngle)
{
	SetRotation(glm::quat(cosf(InAngle / 2), InAxis.x * sinf(InAngle / 2),
		InAxis.y * sinf(InAngle / 2), InAxis.z * sinf(InAngle / 2)));
}

void Transform::SetRotation(const glm::vec3& InRotation)
{
	EulerRotation = InRotation;
	UpdateLocalTransformMatrix();
}

void Transform::SetScale(const glm::vec3& InScale)
{
	Scale = InScale;
	UpdateLocalTransformMatrix();
}

void Transform::SetMatrix4x4(const glm::mat4& InMatrix)
{
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::quat Rotation;
	glm::decompose(InMatrix, Scale, Rotation, Position, skew, perspective);
	SetRotation(Rotation);
	// Won't use skew or perspective.
	UpdateLocalTransformMatrix();
}

glm::vec3 Transform::GetWorldPosition() const
{
	return glm::vec3(GetLocalToWorldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

glm::mat4 Transform::GetLocalToWorldMatrix() const
{
	return GetLocalToAncestorMatrix(nullptr);
}

glm::mat4 Transform::GetLocalToParentMatrix() const
{
	return LocalTransformMatrix;
}

glm::mat4 Transform::GetLocalToAncestorMatrix(SceneNode* InAncestor) const
{
	SceneNode* parent = Node.GetParentPtr();
	if (parent == InAncestor) {
		return LocalTransformMatrix;
	}
	else {
		if (parent == nullptr) {
			throw std::runtime_error("Ancestor does not exist!");
		}
		return parent->GetTransform().GetLocalToAncestorMatrix(InAncestor) *
			LocalTransformMatrix;
	}
}

glm::mat4 Transform::GetCustomToAncestorMatrix(SceneNode* InAncestor, glm::mat4 InCustomMatrix) const
{
	SceneNode* parent = Node.GetParentPtr();
	if (parent == InAncestor) {
		return InCustomMatrix;
	}
	else {
		if (parent == nullptr) {
			throw std::runtime_error("Ancestor does not exist!");
		}
		return parent->GetTransform().GetLocalToAncestorMatrix(InAncestor) *
			InCustomMatrix;
	}
}

glm::vec3 Transform::GetForwardDirection() const
{
	return glm::mat3_cast(GetRotation()) * GetWorldForward();
}

glm::vec3 Transform::GetUpDirection() const
{
	return glm::mat3_cast(GetRotation()) * GetWorldUp();
}

glm::vec3 Transform::GetRightDirection() const
{
	return glm::mat3_cast(GetRotation()) * GetWorldRight();
}

glm::vec3 Transform::GetWorldUp()
{
	return glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 Transform::GetWorldRight()
{
	return glm::vec3(1.f, 0.f, 0.f);
}

glm::vec3 Transform::GetWorldForward()
{
	return glm::vec3(0.f, 0.f, -1.f);
}

void Transform::ApplyKeyframeData(int InFrame)
{
	if (PositionKeyframeTrack.HasKeyframes())
	{
		glm::vec3 pos = PositionKeyframeTrack.GetValueAtFrame(InFrame);
		SetPosition(pos);
	}
	if (RotationKeyframeTrack.HasKeyframes()) 
		SetRotation(RotationKeyframeTrack.GetValueAtFrame(InFrame));
	if (ScaleKeyframeTrack.HasKeyframes()) 
		SetScale(ScaleKeyframeTrack.GetValueAtFrame(InFrame));
}

std::vector<std::string> Transform::GetKeyframeTrackNames() const
{
	std::vector<std::string> names = {"Position", "Rotation", "Scale"};
	return names;
}

void Transform::CreateKeyframeOnTrack(std::string InTrackName, int InFrame)
{
	if (InTrackName == "Position")
	{
		PositionKeyframeTrack.AddKeyframe(InFrame, GetPosition());
	}
	else if (InTrackName == "Rotation")
	{
		RotationKeyframeTrack.AddKeyframe(InFrame, GetEulerRotation());
	}
	else if (InTrackName == "Scale")
	{
		ScaleKeyframeTrack.AddKeyframe(InFrame, GetScale());
	}
}

std::vector<IKeyframeBase*> Transform::GetKeyframesOnTrack(std::string InTrackName)
{
	if (InTrackName == "Position")
	{
		return PositionKeyframeTrack.GetKeyframes();
	}
	else if (InTrackName == "Rotation")
	{
		return RotationKeyframeTrack.GetKeyframes();
	}
	else if (InTrackName == "Scale")
	{
		return ScaleKeyframeTrack.GetKeyframes();
	}

	return std::vector<IKeyframeBase*>();
}

void Transform::UpdateLocalTransformMatrix()
{
	glm::mat4 newMatrix(1.f);

	// Order: scale, rotate, translate
	newMatrix = glm::scale(glm::mat4(1.f), Scale) * newMatrix;
	newMatrix = glm::mat4_cast(GetRotation()) * newMatrix;
	newMatrix = glm::translate(glm::mat4(1.f), Position) * newMatrix;

	LocalTransformMatrix = std::move(newMatrix);
}

}