#pragma once

#include <glm/glm.hpp>

namespace CHISTUDIO {

enum class ELightType {
    Ambient,
    Point,
    Directional,
    Hittable
};

class LightBase {
public:
    LightBase()
        : DiffuseColor(0.8f, 0.8f, 0.8f), SpecularColor(1.0f, 1.0f, 1.0f), bIsLightEnabled(true) {
    }

    virtual ~LightBase() {
    }

    void SetDiffuseColor(const glm::vec3& color) {
        DiffuseColor = color;
    }

    void SetSpecularColor(const glm::vec3& color) {
        SpecularColor = color;
    }

    glm::vec3 GetDiffuseColor() const {
        return DiffuseColor;
    }

    glm::vec3 GetSpecularColor() const {
        return SpecularColor;
    }

    virtual ELightType GetType() const = 0;

    bool IsLightEnabled() const
    {
        return bIsLightEnabled;
    }

    void SetLightEnabled(bool InIsLightEnabled)
    {
        bIsLightEnabled = InIsLightEnabled;
    }

private:
    glm::vec3 DiffuseColor;
    glm::vec3 SpecularColor;

    bool bIsLightEnabled;
};

}