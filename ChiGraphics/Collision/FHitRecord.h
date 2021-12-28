#pragma once

#include <limits>
#include <ostream>

#include <glm/gtx/string_cast.hpp>
#include "ChiGraphics/Materials/Material.h"

namespace CHISTUDIO {

struct FHitRecord 
{
    FHitRecord() : Position(glm::vec3(0.0f)), Normal(glm::vec3(1.0f, 0.0, 0.0f))
    {
        Time = std::numeric_limits<float>::max(); 
    }

    float Time;
    glm::vec3 Position;
    glm::vec3 Normal;
    Material Material_;
};

inline std::ostream& operator<<(std::ostream& os, const FHitRecord& InRecord)
{
    os << "HitRecord <T: " << InRecord.Time << ", P: " << glm::to_string(InRecord.Position) << ", N: " << glm::to_string(InRecord.Normal) << ">";
    return os;
}

}