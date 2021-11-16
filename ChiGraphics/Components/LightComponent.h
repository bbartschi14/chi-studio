#pragma once

#include "ComponentBase.h"
#include <glm/glm.hpp>
#include "ChiGraphics/Lights/LightBase.h"

namespace CHISTUDIO {

class LightComponent : public ComponentBase
{
public:
    LightComponent(std::shared_ptr<LightBase> InLight) : Light(std::move(InLight)) 
    {
    }

    LightBase* GetLightPtr() const {
        return Light.get();
    }

    bool CanCastShadow() const {
        return Light->GetType() == ELightType::Directional;
    }

    ELightType GetLightType() const {
        return Light->GetType();
    }

private:
    std::shared_ptr<LightBase> Light;
};

CREATE_COMPONENT_TRAIT(LightComponent, EComponentType::Light);

}