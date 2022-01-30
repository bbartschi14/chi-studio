#pragma once

#include "../FRay.h"
#include "../FHitRecord.h"
#include "ChiGraphics/RNG.h"

namespace CHISTUDIO {

/** Base interface for creating a hittable object.
 *  Hittable objects can calculate their intersection with a ray,
 *  and sample points on their surface. Matrices are stored
 *  locally to easily transform rays and hit info.
 */
class IHittableBase 
{
public:
    /** Try to intersect the hittable with the given ray. Collisions must occur at least InT_Min on the given ray.
     *  InRecord is modified with collision info. InMaterial allows alpha sampling to be considered for intersection.
     *  It is assumed that ray is in the local coordinates. Returns if an intersection occured.
     */
    virtual bool Intersect(const FRay& InRay, float InT_Min, FHitRecord& InRecord, class Material InMaterial) const = 0;

    /** Sample the surface of the hittable. Note that solid angle sampling is often best. Ref: https://schuttejoe.github.io/post/arealightsampling/ 
     *  Returns the probability of the sampled point.
     */
    virtual float Sample(const glm::vec3& InTargetPoint, glm::vec3& OutPoint, glm::vec3& OutNormal, RNG& InRNG) const = 0;

    virtual ~IHittableBase() {}

    glm::mat4 ModelMatrix;
    glm::mat4 InverseModelMatrix;
    glm::mat4 TransposeInverseModelMatrix;

    class Material Material_;
};

}