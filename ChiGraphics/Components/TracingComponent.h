#pragma once
#include "ComponentBase.h"
#include "ChiGraphics/Collision/Hittables/HittableBase.h"

namespace CHISTUDIO {

enum class ETracingType
{
    Sphere
};

class TracingComponent: public ComponentBase
{

public:
    TracingComponent(std::shared_ptr<IHittableBase> InHittable, ETracingType InTracingType) : Hittable(InHittable), TracingType(InTracingType)
    {
    }

    std::shared_ptr<IHittableBase> Hittable;
    ETracingType TracingType;
};

CREATE_COMPONENT_TRAIT(TracingComponent, EComponentType::Tracing);

}
