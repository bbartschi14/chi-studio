#pragma once

#include "LightBase.h"

namespace CHISTUDIO {
    class PointLight : public LightBase {
    public:
        void SetAttenuation(const glm::vec3& attenuation) {
            Attenuation = attenuation;
        }

        glm::vec3 GetAttenuation() const {
            return Attenuation;
        }

        ELightType GetType() const override {
            return ELightType::Point;
        }

    private:
        glm::vec3 Attenuation;  // (constant, linear, quadratic)
    };
}