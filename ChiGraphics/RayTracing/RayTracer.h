#pragma once

#include <memory>
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "ChiGraphics/Collision/FHitRecord.h"
#include "ChiGraphics/Collision/Hittables/HittableBase.h"
#include <future>

namespace CHISTUDIO {

struct FRayTraceSettings
{
public:
    glm::ivec2 ImageSize;
    size_t MaxBounces;
    glm::vec3 BackgroundColor;
    bool bShadowsEnabled;
    int SamplesPerPixel;
    class FImage* HDRI;
    bool UseHDRI;
    float HDRIStrength;
    bool UseCompositingNodes;
    bool UseIntelDenoise;
};

/** Allows for rendering the scene via ray tracing */
class FRayTracer
{

public:
    FRayTracer(FRayTraceSettings InSettings);

    /** Ray traces the scene, saving the file to the designated filepath, and outputting the image data to OutputTexture */
    std::unique_ptr<class FTexture> Render(const class Scene& InScene, const std::string& InOutputFile);
    
    // Cached settings for the rendering
    FRayTraceSettings Settings;

private:
    // Cached hittables being rendered
    std::vector<std::shared_ptr<IHittableBase>> Hittables;

    // Find all ray-traceable lights in the scene
    std::vector<class LightComponent*> GetLightComponents(const class Scene& InScene);

    // Generates necessary hittable data from objects in the scene. Also adds hittable lights to the lights vector
    void BuildHittableData(const class Scene& InScene, std::vector<class LightComponent*>& InLights);

    // Find the camera to be used for rendering
    std::unique_ptr<class FTracingCamera> GetFirstTracingCamera(const class Scene& InScene);

    // Send a ray into the scene, returning the color result after intersecting and calculating light contributions.
    // Also finds the albedo and normal of the scene at the intersection, used for denoising data.
    glm::dvec3 TraceRay(const class FRay& InRay, size_t InBounces, std::vector<class LightComponent*> InLights, glm::vec3& OutAlbedo, glm::vec3& OutNormal, RNG& InRNG);

    // Return the background color of a ray, used when no hittable is intersected. Can be solid colors, or sampled hdr images.
    glm::vec3 GetBackgroundColor(const glm::vec3& InDirection) const;

    // Calculate light illumination of a single light to a given position. Outputs various data including the overall intensity, direction to light, and distance to light (from the given hit position).
    void GetIllumination(const LightComponent& lightComponent, const glm::dvec3& hitPos, glm::dvec3& directionToLight, glm::dvec3& intensity, double& distanceToLight, RNG& InRNG);

    // Given InRay, find the closest object hit from cached Hittables. Can take in a mask hittable to ignore.
    bool GetClosestObjectHit(const class FRay& InRay, FHitRecord& InRecord, std::shared_ptr<IHittableBase> InHittableToIgnore) const;

    // Used for multithreading, creates data for a single thread to render out a row of pixels.
    void RenderRow(size_t InY, std::vector<LightComponent*>* InLights, FTracingCamera* InTracingCamera, FImage* InOutputImage, FImage* InAlbedoImage, FImage* InNormalImage, int InRNGSeed);

    // Multi-threading
    std::vector<std::future<void>> Futures;
    int RowsComplete;
};

}
