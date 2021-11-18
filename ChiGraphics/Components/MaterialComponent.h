#pragma once

#include "ComponentBase.h"
#include "ChiGraphics/Materials/Material.h"

namespace CHISTUDIO {

class MaterialComponent : public ComponentBase {
public:
    MaterialComponent(std::shared_ptr<Material> InMaterial) {
        SetMaterial(std::move(InMaterial));
    }

    void SetMaterial(std::shared_ptr<Material> InMaterial) {
        Material_ = std::move(InMaterial);
    }

    Material& GetMaterial() {
        return *Material_;
    }

    Material* GetMaterialPtr() {
        return Material_.get();
    }

private:
    std::shared_ptr<Material> Material_;
};

CREATE_COMPONENT_TRAIT(MaterialComponent, EComponentType::Material);

}