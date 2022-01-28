#pragma once

#include "ChiGraphics/Collision/Hittables/HittableBase.h"
#include "ChiGraphics/AliasTypes.h"
#include "ChiGraphics/Collision/Hittables/TriangleHittable.h"
#include "ChiGraphics/Collision/Hittables/Octree.h"

namespace CHISTUDIO {


class MeshHittable: public IHittableBase
{

public:
    MeshHittable(const FPositionArray& positions, const FNormalArray& normals, const FIndexArray& indices, const FTexCoordArray& uvs, bool InUseOctree = true);

    bool Intersect(const FRay& InRay, float Tmin, FHitRecord& InRecord, class Material InMaterial) const override;
    float Sample(const glm::vec3& InTargetPoint, glm::vec3& OutPoint, glm::vec3& OutNormal) const override;

    const std::vector<TriangleHittable>& GetTriangles() const {
        return Triangles;
    }

private:
    std::vector<TriangleHittable> Triangles;
    std::unique_ptr<Octree> Octree_;
    bool bUseOctree;
};

}
