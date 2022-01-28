#include "ChiGraphics\Collision\Hittables\SphereHittable.h"
#include <cmath>
#include <glm/gtx/norm.hpp>
#include "ChiGraphics/Materials/Material.h"

namespace CHISTUDIO {

bool SphereHittable::Intersect(const FRay& InRay, float InT_Min, FHitRecord& InRecord, Material InMaterial) const
{
	// RAY-SPHERE INTERSECTION (https://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/)

	glm::vec3 oc = InRay.GetOrigin() - Origin;

	float a = glm::length2(InRay.GetDirection());
	float b = 2 * glm::dot(oc, InRay.GetDirection());
	float c = glm::length2(oc) - Radius * Radius;

	float discriminant = b * b - 4 * a * c;

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

    // Check if this is the closest hit for the hit record
    if (t < InRecord.Time) {
        InRecord.Time = t;
        InRecord.Position = InRay.At(t);
        InRecord.Normal = glm::normalize(InRecord.Position - Origin);
        return true;
    }

    return false;
}

float SphereHittable::Sample(const glm::vec3& InTargetPoint, glm::vec3& OutPoint, glm::vec3& OutNormal) const
{
    glm::vec2 randomPoint = RandomInUnitDisk();
    float z = glm::sqrt(1.0f - randomPoint.x * randomPoint.x - randomPoint.y * randomPoint.y);
    glm::vec3 normalizedDirection = glm::normalize(InTargetPoint);

    glm::vec3 n1 = std::isnan(normalizedDirection.x) ? glm::normalize(glm::vec3(0.0f, -normalizedDirection.z, normalizedDirection.y)) 
                                                        : glm::normalize(glm::vec3(normalizedDirection.y, -normalizedDirection.x, 0.0f));
    glm::vec3 n2 = glm::cross(n1, normalizedDirection);
    glm::vec3 point = randomPoint.x * n1 + randomPoint.y * n2 + z * normalizedDirection;

    OutPoint = point;
    OutNormal = glm::normalize(point);
    return z / kPi;
}

}