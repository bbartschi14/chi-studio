#pragma once

#include "LightBase.h"

namespace CHISTUDIO {

/** Ambient light to be added evenly across the entire scene. */
class AmbientLight : public LightBase {
    // Use DiffuseColor as the ambient light color.
public:
    void SetAmbientColor(const glm::vec3& color) {
        SetDiffuseColor(color);
    }

    glm::vec3 GetAmbientColor() const {
        return GetDiffuseColor();
    }

    ELightType GetType() const override {
        return ELightType::Ambient;
    }
};

}