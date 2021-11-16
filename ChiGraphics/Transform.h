#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace CHISTUDIO {

class SceneNode;

class Transform
{
public:
    Transform(SceneNode& InNode);
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

private:
    void UpdateLocalTransformMatrix();

    glm::vec3 Position;
    glm::vec3 EulerRotation;
    glm::vec3 Scale;

    glm::mat4 LocalTransformMatrix;

    SceneNode& Node;
};

}
