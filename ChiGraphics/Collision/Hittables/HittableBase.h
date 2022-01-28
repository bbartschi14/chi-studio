#pragma once

#include "../FRay.h"
#include "../FHitRecord.h"

namespace CHISTUDIO {

class IHittableBase 
{
public:
    // It is assumed that ray is in the local coordinates.
    virtual bool Intersect(const FRay& InRay, float InT_Min, FHitRecord& InRecord, class Material InMaterial) const = 0;

    // Sample the surface of the hittable. Note that solid angle sampling is often best. Ref: https://schuttejoe.github.io/post/arealightsampling/
    virtual float Sample(const glm::vec3& InTargetPoint, glm::vec3& OutPoint, glm::vec3& OutNormal) const = 0;

    virtual ~IHittableBase() {}

    glm::mat4 ModelMatrix;
    glm::mat4 InverseModelMatrix;
    glm::mat4 TransposeInverseModelMatrix;

    class Material Material_;
};

}