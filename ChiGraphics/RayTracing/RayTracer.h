#pragma once

#include <memory>
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "ChiGraphics/Collision/FHitRecord.h"
#include "ChiGraphics/Collision/Hittables/MeshHittable.h"

namespace CHISTUDIO {

struct FRayTraceSettings
{
public:
    glm::ivec2 ImageSize;
    size_t MaxBounces;
    glm::vec3 BackgroundColor;
    bool bShadowsEnabled;
};

/** Allows for rendering the scene via ray tracing */
class FRayTracer
{

public:
    FRayTracer(FRayTraceSettings InSettings);

    /** Ray traces the scene, saving the file to the designated filepath, and outputting the image data to OutputTexture */
    std::unique_ptr<class FTexture> Render(const class Scene& InScene, const std::string& InOutputFile);

    FRayTraceSettings Settings;
private:
    std::vector<std::unique_ptr<MeshHittable>> Hittables;

    std::vector<class LightComponent*> GetLightComponents(const class Scene& InScene);
    void BuildHittableData(const class Scene& InScene);
    std::unique_ptr<class FTracingCamera> GetFirstTracingCamera(const class Scene& InScene);
    glm::vec3 TraceRay(const class FRay& InRay, size_t InBounces, FHitRecord& InRecord, std::vector<class LightComponent*> InLights) const;
    glm::vec3 GetBackgroundColor(const glm::vec3& InDirection) const;
    void GetIllumination(const LightComponent& lightComponent, const glm::vec3& hitPos, glm::vec3& directionToLight, glm::vec3& intensity, float& distanceToLight) const;
};

}
