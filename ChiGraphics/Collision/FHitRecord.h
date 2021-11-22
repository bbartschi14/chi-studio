#pragma once

#include <limits>
#include <ostream>

#include <glm/gtx/string_cast.hpp>
#include "ChiGraphics/Materials/Material.h"

namespace CHISTUDIO {

struct FHitRecord 
{
    FHitRecord()
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