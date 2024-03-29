#pragma once

#include "LightBase.h"
#include <memory>
namespace CHISTUDIO {

/** Represents a light that uses a hittable to emit light. */
class HittableLight : public LightBase {
public:
    HittableLight::HittableLight()
    {

    }

    ELightType GetType() const override {
        return ELightType::Hittable;
    }

    std::shared_ptr<class IHittableBase> GetHittable()
    {
        return Hittable;
    }

    void SetHittable(std::shared_ptr<class IHittableBase> InHittable)
    {
        Hittable = InHittable;
    }

private:
    std::shared_ptr<class IHittableBase> Hittable;
};

}
