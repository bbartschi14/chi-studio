#include <cmath>
#include <glm/gtx/norm.hpp>
#include "CylinderHittable.h"
#include "ChiGraphics/Materials/Material.h"

namespace CHISTUDIO {

bool CylinderHittable::Intersect(const FRay& InRay, float InT_Min, FHitRecord& InRecord, Material InMaterial) const
{
    // Works without having to transform the cylinders direction 

    glm::vec3 cylinderLengthVector = glm::normalize(Direction) * Length;
    glm::vec3 rayToCylinderOrigin = InRay.GetOrigin() - Origin;
    glm::vec3 rayOriginToCylinderPerpendicular = glm::cross(rayToCylinderOrigin, cylinderLengthVector);
    glm::vec3 rayDirectionToCylinderPerpendicular = glm::cross(InRay.GetDirection(), cylinderLengthVector);

    float a = glm::length2(rayDirectionToCylinderPerpendicular);
    float b = 2 * glm::dot(rayDirectionToCylinderPerpendicular, rayOriginToCylinderPerpendicular);
    float c = glm::length2(rayOriginToCylinderPerpendicular) - (Radius * Radius * glm::length2(cylinderLengthVector));
	float discriminant = b * b - 4 * (a * c);

    if (discriminant < 0) {
        // No hit
        return false;
    }

    discriminant = sqrt(discriminant);

    // Get the two T's of sphere intersection
    float tPlus = (-b + discriminant) / (2 * a);
    float tMinus = (-b - discriminant) / (2 * a);

    // Find the closest possible T for intersection
    float t;
    if (tMinus < InT_Min) {
        if (tPlus < InT_Min)
            return false;
        else {
            t = tPlus;
        }
    }
    else {
        t = tMinus;
    }

    glm::vec3 intersectionPoint = InRay.At(t);
    glm::vec3 cylinderToIntersection = intersectionPoint - Origin;
    float intersectionProjectedToCylinderAxis = glm::dot(cylinderToIntersection, cylinderLengthVector) / glm::length2(cylinderLengthVector);

    if (intersectionProjectedToCylinderAxis >= 0.0f && intersectionProjectedToCylinderAxis <= 1.0f)
    {
        // Check if this is the closest hit for the hit record
        if (t < InRecord.Time) {
            InRecord.Time = t;
            InRecord.Position = intersectionPoint;
            //InRecord.Normal = glm::normalize(InRecord.Position - Origin);
            return true;
        }
    }

	return false;
}

float CylinderHittable::Sample(const glm::vec3& InTargetPoint, glm::vec3& OutPoint, glm::vec3& OutNormal, RNG& InRNG) const
{
    return 0.0f;
}

}