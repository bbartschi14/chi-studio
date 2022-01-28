#pragma once
#include "HittableBase.h"

namespace CHISTUDIO {

/** Implements simple ray-sphere intersection. */
class SphereHittable : public IHittableBase
{
public:
	SphereHittable(float InRadius, glm::vec3 InOrigin) : Radius(InRadius), Origin(InOrigin) {}

	bool Intersect(const FRay& InRay, float InT_Min, FHitRecord& InRecord, class Material InMaterial) const override;

	/**
	* Samples for a point on the closest hemisphere to the target point, weighted by the cosine.
	* Doesn't fully implement solid angle sampling.
	*/
	float Sample(const glm::vec3& InTargetPoint, glm::vec3& OutPoint, glm::vec3& OutNormal) const override;
private:
	float Radius;
	glm::vec3 Origin;
};

}

