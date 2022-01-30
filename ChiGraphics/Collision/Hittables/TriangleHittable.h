#pragma once
#include "HittableBase.h"
#include <vector>

namespace CHISTUDIO {

/** Implements ray-triangle intersection. */
class TriangleHittable : public IHittableBase
{
public:
    TriangleHittable(const glm::vec3& InPos0, const glm::vec3& InPos1, const glm::vec3& InPos2,
        const glm::vec3& InNorm0, const glm::vec3& InNorm1, const glm::vec3& InNorm2, const glm::vec2& InUV1, const glm::vec2& InUV2, const glm::vec2& InUV3 );
    TriangleHittable(const std::vector<glm::vec3>& InPositions, const std::vector<glm::vec3>& InNormals, const std::vector<glm::vec2>& InUVs);

	bool Intersect(const FRay& InRay, float InT_Min, FHitRecord& InRecord, class Material InMaterial) const override;
    float Sample(const glm::vec3& InTargetPoint, glm::vec3& OutPoint, glm::vec3& OutNormal, RNG& InRNG) const override;

    glm::vec3 GetPosition(size_t i) const {
        return Positions[i];
    }
    glm::vec3 GetNormal(size_t i) const {
        return Normals[i];
    }
    glm::vec2 GetUV(size_t i) const {
        return UVs[i];
    }

private:
	std::vector<glm::vec3> Positions;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> UVs;
};

}
