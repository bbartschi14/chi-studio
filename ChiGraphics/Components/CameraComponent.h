#pragma once

#include "ComponentBase.h"
#include <glm/glm.hpp>
#include "ChiGraphics/SceneNode.h"
namespace CHISTUDIO {

class CameraComponent : public ComponentBase
{
public:
    CameraComponent(float InFOV, float InAspectRatio, float InZNear, float InZFar, float InFocusDistance = 5.0f, float InAperture = 0.0f);
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewMatrix() const;

    void SetAspectRatio(float InAspectRatio) {
        AspectRatio = InAspectRatio;
    }

    void SetAspectRatio(uint32_t width, uint32_t height) {
        AspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }

    void SetViewMatrix(std::unique_ptr<glm::mat4> InViewMatrix) {
        ViewMatrix = std::move(InViewMatrix);
    }

    glm::vec3 GetAbsoluteCameraPosition() const {
        if (ViewMatrix)
        {
            // TODO: Optimize to calculate inverse view matrix when the view matrix is set, then store it
            glm::mat4 viewMatrix = *ViewMatrix;
            glm::mat4 invViewMatrix = glm::inverse(viewMatrix);
            glm::vec4 result = invViewMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f);
            return glm::vec3(result.x, result.y, result.z);
        }
        
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    class FRay GenerateRay(glm::vec2 InMousePosition, glm::vec2 InViewportSize);

    float GetFOV() const
    {
        return FOV;
    }

    float GetAspectRatio() const
    {
        return AspectRatio;
    }

    float FocusDistance;
    float Aperture;

private:
    glm::vec2 GetNormalizedDeviceCoords(glm::vec2 InMousePosition, glm::vec2 InViewportSize);
    glm::vec4 GetEyeCoords(glm::vec4 InClipCoords);
    glm::vec3 GetWorldCoords(glm::vec4 InEyeCoords);

    float FOV;
    float AspectRatio;
    float ZNear;
    float ZFar;
    

    std::unique_ptr<glm::mat4> ViewMatrix;
};

CREATE_COMPONENT_TRAIT(CameraComponent, EComponentType::Camera);

}

