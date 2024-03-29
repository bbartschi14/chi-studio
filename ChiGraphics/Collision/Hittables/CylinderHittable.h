#pragma once
#include "HittableBase.h"

namespace CHISTUDIO {

/** Implements ray-cylinder intersection. Useful for implementing line selection in the viewport. */
class CylinderHittable : public IHittableBase
{
public:
	CylinderHittable(float InRadius, glm::vec3 InOrigin, glm::vec3 InDirection, float InLength) : Radius(InRadius), Origin(InOrigin), Direction(InDirection), Length(InLength) {}

	bool Intersect(const FRay& InRay, float InT_Min, FHitRecord& InRecord, class Material InMaterial) const override;
	float Sample(const glm::vec3& InTargetPoint, glm::vec3& OutPoint, glm::vec3& OutNormal, RNG& InRNG) const override;

private:
	float Radius;
	glm::vec3 Origin;
	glm::vec3 Direction;
	float Length;
};

}

