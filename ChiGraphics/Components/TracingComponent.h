#pragma once
#include "ComponentBase.h"
#include "ChiGraphics/Collision/Hittables/HittableBase.h"

namespace CHISTUDIO {

class TracingComponent: public ComponentBase
{

public:
    TracingComponent(std::shared_ptr<IHittableBase> InHittable) : Hittable(InHittable)
    {
    }

    std::shared_ptr<IHittableBase> Hittable;
};

CREATE_COMPONENT_TRAIT(TracingComponent, EComponentType::Tracing);

}
