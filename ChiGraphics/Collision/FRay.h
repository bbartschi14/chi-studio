#pragma once

#include <ostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace CHISTUDIO {

/** Simple representation of a mathematical ray (origin point and direction)
*   Includes helper methods for transforming the ray, as it is typically used
*   for raycasting/intersection detection, so it can be transformed into local object
*   spaces.
*/
class FRay 
{
public:
    FRay(const glm::vec3& InOrigin, const glm::vec3& InDirection)
        : Origin(InOrigin), Direction(InDirection) 
    {
    }

    const glm::vec3& GetOrigin() const 
    {
        return Origin;
    }

    const glm::vec3& GetDirection() const 
    {
        return Direction;
    }

    void SetDirection(const glm::vec3& InDirection) 
    {
        Direction = InDirection;
    }

    glm::vec3 At(float t) const 
    {
        return Origin + t * Direction;
    }

    void ApplyTransform(const glm::mat4& InTransform) 
    {

        glm::vec3 referencePoint = At(1.0f);
        glm::vec4 newOrigin = InTransform * glm::vec4(Origin, 1.0f);
        Origin = glm::vec3(newOrigin / newOrigin.w);
        glm::vec4 newReferencePoint = InTransform * glm::vec4(referencePoint, 1.0f);
        referencePoint = glm::vec3(newReferencePoint / newReferencePoint.w);
        Direction = referencePoint - Origin;

        // Note: do not normalize Direction here since we want
        // referencePoint to always be At(1.0) before/after transform.
    }

private:
    glm::vec3 Origin;
    glm::vec3 Direction;
};

inline std::ostream& operator<<(std::ostream& os, const FRay& InRay) 
{
    os << "Ray " << glm::to_string(InRay.GetOrigin()) << "->"
        << glm::to_string(InRay.GetDirection());
    return os;
}

}