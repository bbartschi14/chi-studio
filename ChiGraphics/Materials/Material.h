#pragma once

#include <glm/glm.hpp>

namespace CHISTUDIO {
    
class Material 
{
public:
    Material()
        : AmbientColor(glm::vec3(0.7f)),
        DiffuseColor(glm::vec3(0.7f)),
        SpecularColor(glm::vec3(0.15f)),
        Shininess(1.0f) {
    }

    Material(const glm::vec3& InAmbientColor,
        const glm::vec3& InDiffuseColor,
        const glm::vec3& InSpecularColor,
        float InShininess)
        : AmbientColor(InAmbientColor),
        DiffuseColor(InDiffuseColor),
        SpecularColor(InSpecularColor),
        Shininess(InShininess) {
    }

    static const Material& GetDefault() {
        static Material default_material(glm::vec3(0.7f),
            glm::vec3(0.7f),
            glm::vec3(0.15f), 1.0f);
        return default_material;
    }

    glm::vec3 GetAmbientColor() const {
        return AmbientColor;
    }

    void SetAmbientColor(const glm::vec3& color) {
        AmbientColor = color;
    }

    glm::vec3 GetDiffuseColor() const {
        return DiffuseColor;
    }

    void SetDiffuseColor(const glm::vec3& color) {
        DiffuseColor = color;
    }

    glm::vec3 GetSpecularColor() const {
        return SpecularColor;
    }

    void SetSpecularColor(const glm::vec3& color) {
        SpecularColor = color;
    }

    float GetShininess() const {
        return Shininess;
    }

    void SetShininess(float InShininess) {
        Shininess = InShininess;
    }

private:
    glm::vec3 AmbientColor;
    glm::vec3 DiffuseColor;
    glm::vec3 SpecularColor;
    float Shininess;
};

}