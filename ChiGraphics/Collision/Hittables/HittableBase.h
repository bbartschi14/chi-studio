#pragma once

#include "../FRay.h"
#include "../FHitRecord.h"

namespace CHISTUDIO {

class IHittableBase 
{
public:
    // It is assumed that ray is in the local coordinates.
    virtual bool Intersect(const FRay& InRay, float InT_Min, FHitRecord& InRecord) const = 0;
    virtual ~IHittableBase() {}

    glm::mat4 ModelMatrix;
    glm::mat4 InverseModelMatrix;
    glm::mat4 TransposeInverseModelMatrix;

    class Material Material_;
};

}