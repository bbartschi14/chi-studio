#pragma once
#include "HittableBase.h"
#include <vector>

namespace CHISTUDIO {

/** Implements ray-cylinder intersection. */
class TriangleHittable : public IHittableBase
{
public:
    TriangleHittable(const glm::vec3& InPos0, const glm::vec3& InPos1, const glm::vec3& InPos2,
        const glm::vec3& InNorm0, const glm::vec3& InNorm1, const glm::vec3& InNorm2);
    TriangleHittable(const std::vector<glm::vec3>& InPositions, const std::vector<glm::vec3>& InNormals);

	bool Intersect(const FRay& InRay, float InT_Min, FHitRecord& InRecord) const override;
    float Sample(const glm::vec3& InTargetPoint, glm::vec3& OutPoint, glm::vec3& OutNormal) const override;

    glm::vec3 GetPosition(size_t i) const {
        return Positions[i];
    }
    glm::vec3 GetNormal(size_t i) const {
        return Normals[i];
    }

private:
	std::vector<glm::vec3> Positions;
	std::vector<glm::vec3> Normals;
};

}
