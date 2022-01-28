#pragma once

#include "LightBase.h"

namespace CHISTUDIO {
    class DirectionalLight : public LightBase {
    public:
        glm::vec3 BaseDirection;

        ELightType GetType() const override {
            return ELightType::Directional;
        }
    };
}