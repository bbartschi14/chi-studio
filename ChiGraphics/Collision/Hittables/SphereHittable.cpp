#include "ChiGraphics\Collision\Hittables\SphereHittable.h"
#include <cmath>
#include <glm/gtx/norm.hpp>

namespace CHISTUDIO {

bool SphereHittable::Intersect(const FRay& InRay, float InT_Min, FHitRecord& InRecord) const
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

}