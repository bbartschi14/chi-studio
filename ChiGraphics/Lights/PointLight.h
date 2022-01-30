#pragma once

#include "LightBase.h"

namespace CHISTUDIO {
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