#include "Transform.h"

#include <memory>
#include <stdexcept>
#include <glm/gtx/matrix_decompose.hpp>
#include "ChiGraphics/Utilities.h"
#include "ChiGraphics/SceneNode.h"
#include <glm/gtx/string_cast.hpp>

namespace CHISTUDIO {

Transform::Transform(SceneNode& InNode)
	: IKeyframeable("Transform"), Position(0.f), EulerRotation(glm::vec3(0.f, 0.f, 0.f)), Scale(glm::vec3(1.f)), Node(InNode)
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
	glm::vec3 originalPos = GetPosition();
	bool bPosModified = false;
	if (PositionXKeyframeTrack.HasKeyframes())
	{
		bPosModified = true;
		originalPos.x = PositionXKeyframeTrack.GetValueAtFrame(InFrame);
	}
	if (PositionYKeyframeTrack.HasKeyframes())
	{
		bPosModified = true;
		originalPos.y = PositionYKeyframeTrack.GetValueAtFrame(InFrame);
	}
	if (PositionZKeyframeTrack.HasKeyframes())
	{
		bPosModified = true;
		originalPos.z = PositionZKeyframeTrack.GetValueAtFrame(InFrame);
	}
	if (bPosModified)
	{
		SetPosition(originalPos);
	}

	glm::vec3 originalRot = GetEulerRotation();
	bool bRotModified = false;
	if (RotationXKeyframeTrack.HasKeyframes())
	{
		bRotModified = true;
		originalPos.x = RotationXKeyframeTrack.GetValueAtFrame(InFrame);
	}
	if (RotationYKeyframeTrack.HasKeyframes())
	{
		bRotModified = true;
		originalPos.y = RotationYKeyframeTrack.GetValueAtFrame(InFrame);
	}
	if (RotationZKeyframeTrack.HasKeyframes())
	{
		bRotModified = true;
		originalPos.z = RotationZKeyframeTrack.GetValueAtFrame(InFrame);
	}
	if (bRotModified)
	{
		SetRotation(originalRot);
	}

	glm::vec3 originalScale = GetScale();
	bool bScaleModified = false;
	if (ScaleXKeyframeTrack.HasKeyframes())
	{
		bScaleModified = true;
		originalScale.x = ScaleXKeyframeTrack.GetValueAtFrame(InFrame);
	}
	if (ScaleYKeyframeTrack.HasKeyframes())
	{
		bScaleModified = true;
		originalScale.y = ScaleYKeyframeTrack.GetValueAtFrame(InFrame);
	}
	if (ScaleZKeyframeTrack.HasKeyframes())
	{
		bScaleModified = true;
		originalScale.z = ScaleZKeyframeTrack.GetValueAtFrame(InFrame);
	}
	if (bScaleModified)
	{
		SetScale(originalScale);
	}
}

std::vector<std::string> Transform::GetKeyframeTrackNames() const
{
	std::vector<std::string> names = {"Position X", "Position Y", "Position Z", "Rotation X", "Rotation Y", "Rotation Z", "Scale X", "Scale Y", "Scale Z"};
	return names;
}

void Transform::CreateKeyframeOnTrack(std::string InTrackName, int InFrame)
{
	if (InTrackName == "Position X")
	{
		PositionXKeyframeTrack.AddKeyframe(InFrame, GetPosition().x);
	}
	else if (InTrackName == "Position Y")
	{
		PositionYKeyframeTrack.AddKeyframe(InFrame, GetPosition().y);
	}
	else if (InTrackName == "Position Z")
	{
		PositionZKeyframeTrack.AddKeyframe(InFrame, GetPosition().z);
	}
	else if (InTrackName == "Rotation X")
	{
		RotationXKeyframeTrack.AddKeyframe(InFrame, GetEulerRotation().x);
	}
	else if (InTrackName == "Rotation Y")
	{
		RotationYKeyframeTrack.AddKeyframe(InFrame, GetEulerRotation().y);
	}
	else if (InTrackName == "Rotation Z")
	{
		RotationZKeyframeTrack.AddKeyframe(InFrame, GetEulerRotation().z);
	}
	else if (InTrackName == "Scale X")
	{
		ScaleXKeyframeTrack.AddKeyframe(InFrame, GetScale().x);
	}
	else if (InTrackName == "Scale Y")
	{
		ScaleYKeyframeTrack.AddKeyframe(InFrame, GetScale().y);
	}
	else if (InTrackName == "Scale Z")
	{
		ScaleZKeyframeTrack.AddKeyframe(InFrame, GetScale().z);
	}
}

void Transform::DeleteKeyframeOnTrack(std::string InTrackName, int InIndex)
{
	if (InTrackName == "Position X")
	{
		PositionXKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
	}
	else if (InTrackName == "Position Y")
	{
		PositionYKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
	}
	else if (InTrackName == "Position Z")
	{
		PositionZKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
	}
	else if (InTrackName == "Rotation X")
	{
		RotationXKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
	}
	else if (InTrackName == "Rotation Y")
	{
		RotationYKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
	}
	else if (InTrackName == "Rotation Z")
	{
		RotationZKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
	}
	else if (InTrackName == "Scale X")
	{
		ScaleXKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
	}
	else if (InTrackName == "Scale Y")
	{
		ScaleYKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
	}
	else if (InTrackName == "Scale Z")
	{
		ScaleZKeyframeTrack.DeleteKeyframeAtIndex(InIndex);
	}
}

std::vector<IKeyframeBase*> Transform::GetKeyframesOnTrack(std::string InTrackName)
{
	if (InTrackName == "Position X")
	{
		return PositionXKeyframeTrack.GetKeyframes();
	}
	else if (InTrackName == "Position Y")
	{
		return PositionYKeyframeTrack.GetKeyframes();
	}
	else if (InTrackName == "Position Z")
	{
		return PositionZKeyframeTrack.GetKeyframes();
	}
	else if (InTrackName == "Rotation X")
	{
		return RotationXKeyframeTrack.GetKeyframes();
	}
	else if (InTrackName == "Rotation Y")
	{
		return RotationYKeyframeTrack.GetKeyframes();
	}
	else if (InTrackName == "Rotation Z")
	{
		return RotationZKeyframeTrack.GetKeyframes();
	}
	else if (InTrackName == "Scale X")
	{
		return ScaleXKeyframeTrack.GetKeyframes();
	}
	else if (InTrackName == "Scale Y")
	{
		return ScaleYKeyframeTrack.GetKeyframes();
	}
	else if (InTrackName == "Scale Z")
	{
		return ScaleZKeyframeTrack.GetKeyframes();
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