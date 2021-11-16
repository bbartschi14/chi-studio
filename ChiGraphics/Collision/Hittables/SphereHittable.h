#pragma once
#include "HittableBase.h"

namespace CHISTUDIO {

/** Implements simple ray-sphere intersection. */
class SphereHittable : public IHittableBase
{
public:
	SphereHittable(float InRadius, glm::vec3 InOrigin) : Radius(InRadius), Origin(InOrigin) {}

	bool Intersect(const FRay& InRay, float InT_Min, FHitRecord& InRecord) const override;

private:
	float Radius;
	glm::vec3 Origin;
};

}

