#pragma once

#include "LightBase.h"

namespace CHISTUDIO {

/** Emits light from a single point in all directions. Has inverse-squared falloff and
 * Can specify a non-zero radius for a larger light source.
 */
class PointLight : public LightBase {
public:
    ELightType GetType() const override {
        return ELightType::Point;
    }

    void SetRadius(const float& InRadius) {
        Radius = InRadius;
    }

    float GetRadius() const {
        return Radius;
    }

private:
    float Radius;
};

}