#pragma once

#include <cmath>
#include <glm/ext/quaternion_geometric.hpp>
#include "../Utilities.h"
#include "ChiGraphics/Collision/FRay.h"

namespace CHISTUDIO {

struct FCameraSpec {
    glm::vec3 Center;
    glm::vec3 Direction;
    glm::vec3 Up;
    float FOV_Degrees;
    float AspectRatio; // Height / Width
};

class FTracingCamera {
public:
    FTracingCamera(const FCameraSpec& InSpec) {
        Center = InSpec.Center;
        Direction = glm::normalize(InSpec.Direction);
        Up = glm::normalize(InSpec.Up);
        FOV_Radian = ToRadian(InSpec.FOV_Degrees);
        Horizontal = glm::normalize(glm::cross(Direction, Up));
        AspectRatio = InSpec.AspectRatio;
    }

    FRay GenerateRay(const glm::vec2& point) {
        float d = 1.0f / tanf(FOV_Radian / 2.0f);
        glm::vec3 new_dir = d * Direction + point[0] * Horizontal  + point[1] * Up * AspectRatio;
        new_dir = glm::normalize(new_dir);

        return FRay(Center, new_dir);
    }

    float GetTMin() const {
        return 0.0f;
    }

private:
    glm::vec3 Center;
    glm::vec3 Direction;
    glm::vec3 Up;
    float FOV_Radian;
    float AspectRatio;
    glm::vec3 Horizontal;
};

}