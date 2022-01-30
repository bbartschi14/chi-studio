#pragma once

#include "LightBase.h"

namespace CHISTUDIO {

/** Emits light in a single direction from an infinitely far point. TODO: Implement angular diameter */
class DirectionalLight : public LightBase {
public:
    glm::vec3 BaseDirection;

    ELightType GetType() const override {
        return ELightType::Directional;
    }
};

}