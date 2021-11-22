#pragma once

#include <memory>
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "ChiGraphics/Collision/FHitRecord.h"
#include "ChiGraphics/Collision/Hittables/HittableBase.h"

namespace CHISTUDIO {

struct FRayTraceSettings
{
public:
    glm::ivec2 ImageSize;
    size_t MaxBounces;
    glm::vec3 BackgroundColor;
    bool bShadowsEnabled;
    int SamplesPerPixel;
};

/** Allows for rendering the scene via ray tracing */
class FRayTracer
{

public:
    int debugNANCount;
    int debugIndirectCount;
    glm::vec3 debugAverageIndirect;
    bool bDebug;

    FRayTracer(FRayTraceSettings InSettings);

    /** Ray traces the scene, saving the file to the designated filepath, and outputting the image data to OutputTexture */
    std::unique_ptr<class FTexture> Render(const class Scene& InScene, const std::string& InOutputFile);

    FRayTraceSettings Settings;
private:
    std::vector<std::shared_ptr<IHittableBase>> Hittables;

    std::vector<class LightComponent*> GetLightComponents(const class Scene& InScene);
    void BuildHittableData(const class Scene& InScene);
    std::unique_ptr<class FTracingCamera> GetFirstTracingCamera(const class Scene& InScene);
    glm::dvec3 TraceRay(const class FRay& InRay, size_t InBounces, std::vector<class LightComponent*> InLights);
    glm::vec3 GetBackgroundColor(const glm::vec3& InDirection) const;
    void GetIllumination(const LightComponent& lightComponent, const glm::dvec3& hitPos, glm::dvec3& directionToLight, glm::dvec3& intensity, double& distanceToLight);
    bool GetClosestObjectHit(const class FRay& InRay, FHitRecord& InRecord) const;
};

}
