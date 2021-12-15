#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "ChiGraphics/Keyframing/Keyframeable.h"
#include "ChiGraphics/Keyframing/Keyframe.h"
#include "ChiGraphics/Keyframing/KeyframeTrack.h"

namespace CHISTUDIO {

class SceneNode;

class Transform : public IKeyframeable
{
public:
    Transform(SceneNode& InNode);
    ~Transform();
    void SetPosition(const glm::vec3& InPosition);
    void SetRotation(const glm::quat& InRotation);
    void SetRotation(const glm::vec3& InAxis, float InAngle);
    void SetRotation(const glm::vec3& InRotation);

    void SetScale(const glm::vec3& InScale);
    void SetMatrix4x4(const glm::mat4& InMatrix);

    glm::vec3 GetPosition() const {
        return Position;
    }

    glm::quat GetRotation() const {
        return glm::quat(glm::radians(EulerRotation));
    }

    glm::vec3 GetEulerRotation() const {
        return EulerRotation;
    }

    glm::vec3 GetScale() const {
        return Scale;
    }

    glm::vec3 GetWorldPosition() const;
    glm::mat4 GetLocalToWorldMatrix() const;
    glm::mat4 GetLocalToParentMatrix() const;
    glm::mat4 GetLocalToAncestorMatrix(SceneNode* InAncestor) const;
    glm::mat4 GetCustomToAncestorMatrix(SceneNode* InAncestor, glm::mat4 InCustomMatrix) const;

    glm::vec3 GetForwardDirection() const;
    glm::vec3 GetUpDirection() const;
    glm::vec3 GetRightDirection() const;
    static glm::vec3 GetWorldUp();
    static glm::vec3 GetWorldRight();
    static glm::vec3 GetWorldForward();

    // Begin Keyframeable
    void ApplyKeyframeData(int InFrame) override;
    std::vector<std::string> GetKeyframeTrackNames() const override;
    void CreateKeyframeOnTrack(std::string InTrackName, int InFrame) override;
    void DeleteKeyframeOnTrack(std::string InTrackName, int InIndex) override;
    std::vector<IKeyframeBase*> GetKeyframesOnTrack(std::string InTrackName) override;
    // ~ End Keyframeable

private:
    void UpdateLocalTransformMatrix();

    glm::vec3 Position;
    glm::vec3 EulerRotation;
    glm::vec3 Scale;

    glm::mat4 LocalTransformMatrix;

    FKeyframeTrack<float> PositionXKeyframeTrack;
    FKeyframeTrack<float> PositionYKeyframeTrack;
    FKeyframeTrack<float> PositionZKeyframeTrack;
    FKeyframeTrack<float> RotationXKeyframeTrack;
    FKeyframeTrack<float> RotationYKeyframeTrack;
    FKeyframeTrack<float> RotationZKeyframeTrack;
    FKeyframeTrack<float> ScaleXKeyframeTrack;
    FKeyframeTrack<float> ScaleYKeyframeTrack;
    FKeyframeTrack<float> ScaleZKeyframeTrack;

    SceneNode& Node;
};

}
