#pragma once

#include "ComponentBase.h"
#include "ChiGraphics/Materials/Material.h"

namespace CHISTUDIO {

class MaterialComponent : public ComponentBase {
public:
    MaterialComponent(std::shared_ptr<Material> InMaterial, bool IsDebugMaterial = false) : bIsDebugMaterial(IsDebugMaterial)
    {
        SetMaterial(std::move(InMaterial));
    }

    void SetMaterial(std::shared_ptr<Material> InMaterial) {
        Material_ = std::move(InMaterial);
    }

    Material& GetMaterial() {
        return *Material_;
    }

    std::shared_ptr<Material> GetMaterialShared()
    {
        return Material_;
    }

    Material* GetMaterialPtr() {
        return Material_.get();
    }

    bool bIsDebugMaterial;
private:
    std::shared_ptr<Material> Material_;
};

CREATE_COMPONENT_TRAIT(MaterialComponent, EComponentType::Material);

}