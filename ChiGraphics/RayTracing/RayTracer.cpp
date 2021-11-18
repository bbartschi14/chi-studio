#include "RayTracer.h"
#include "ChiGraphics/GL_Wrapper/FTexture.h"
#include "ChiGraphics/Scene.h"
#include "ChiGraphics/Collision/Hittables/MeshHittable.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/Components/LightComponent.h"
#include "ChiGraphics/Components/MaterialComponent.h"
#include "ChiGraphics/RayTracing/FTracingCamera.h"
#include "ChiGraphics/Cameras/TracingCameraNode.h"
#include "ChiGraphics/GL_Wrapper/FTexture.h"
#include "ChiGraphics/Utilities.h"
#include "ChiGraphics/Lights/PointLight.h"
#include "ChiGraphics/Lights/AmbientLight.h"

namespace CHISTUDIO {

FRayTracer::FRayTracer(FRayTraceSettings InSettings)
	: Settings(InSettings)
{
}

std::unique_ptr<FTexture> FRayTracer::Render(const Scene& InScene, const std::string& InOutputFile)
{
	auto OutputTexture = make_unique<FTexture>();
	OutputTexture->Reserve(GL_RGB, Settings.ImageSize.x, Settings.ImageSize.y, GL_RGBA, GL_UNSIGNED_BYTE);

	std::unique_ptr<FTracingCamera> tracingCamera = GetFirstTracingCamera(InScene);
	if (tracingCamera == nullptr)
	{
		std::cout << "No tracing camera" << std::endl;
		return OutputTexture;
	}
		

	auto lightComponents = GetLightComponents(InScene);
	BuildHittableData(InScene);

	FImage outputImage(Settings.ImageSize.x, Settings.ImageSize.y);

	for (size_t y = 0; y < Settings.ImageSize.y; y++) {
		for (size_t x = 0; x < Settings.ImageSize.x; x++) {
			// Set coords from [ -1, 1 ] for both x and y.
			float cameraX = (float(x) / (Settings.ImageSize.x - 1)) * 2 - 1;
			float cameraY = (float(y) / (Settings.ImageSize.y - 1)) * 2 - 1;

			glm::vec3 color(0.f);
			// Use camera coords to generate a ray into the scene
		
			FRay cameraToSceneRay = tracingCamera->GenerateRay(glm::vec2(cameraX, cameraY));
			FHitRecord hitRecord;
			color += TraceRay(cameraToSceneRay, Settings.MaxBounces, hitRecord, lightComponents);
			
			glm::vec3 gammaCorrectedColor = glm::vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));
			outputImage.SetPixel(x, y, gammaCorrectedColor);
		}
		std::cout << "Rendered: " << (float)y / Settings.ImageSize.y * 100 << " %" << std::endl;
	}

	if (InOutputFile.size())
		outputImage.SavePNG(InOutputFile);

	// Send pixel data to output texture for viewing
	OutputTexture->UpdateImage(outputImage);

	return OutputTexture;
}

std::vector<LightComponent*> FRayTracer::GetLightComponents(const Scene& InScene)
{
	auto& root = InScene.GetRootNode();
	std::vector<LightComponent*> lightComps = root.GetComponentPtrsInChildren<LightComponent>();

	return lightComps;
}

void FRayTracer::BuildHittableData(const Scene& InScene)
{
	Hittables.clear();

	auto& root = InScene.GetRootNode();
	std::vector<RenderingComponent*> renderingComps = root.GetComponentPtrsInChildren<RenderingComponent>();

	std::vector<std::unique_ptr<MeshHittable>> meshHittables;
	for (RenderingComponent* renderingComp : renderingComps)
	{
		if (!renderingComp->bIsDebugRender)
		{
			VertexObject* vertexObject = renderingComp->GetVertexObjectPtr();
			std::unique_ptr<MeshHittable> hittable = make_unique<MeshHittable>(vertexObject->GetPositions(), vertexObject->GetNormals(), vertexObject->GetIndices());

			hittable->ModelMatrix = renderingComp->GetNodePtr()->GetTransform().GetLocalToWorldMatrix();
			hittable->InverseModelMatrix = glm::inverse(hittable->ModelMatrix);
			hittable->TransposeInverseModelMatrix = glm::transpose(hittable->InverseModelMatrix);

			if (auto materialComp = renderingComp->GetNodePtr()->GetComponentPtr<MaterialComponent>())
			{
				hittable->Material_ = materialComp->GetMaterial();
			}
			else
			{
				hittable->Material_ = Material::GetDefault();
			}

			Hittables.emplace_back(std::move(hittable));
		}
	}
}

std::unique_ptr<FTracingCamera> FRayTracer::GetFirstTracingCamera(const Scene& InScene)
{
	auto& root = InScene.GetRootNode();
	std::vector<CameraComponent*> cameraComps = root.GetComponentPtrsInChildren<CameraComponent>();

	for (CameraComponent* cameraComp : cameraComps)
	{
		SceneNode* node = cameraComp->GetNodePtr();
		TracingCameraNode* tracingCameraNode = dynamic_cast<TracingCameraNode*>(node);
		if (tracingCameraNode)
		{
			return tracingCameraNode->GetTracingCamera(Settings.ImageSize);
		}
	}

	return nullptr;
}

glm::vec3 FRayTracer::TraceRay(const FRay& InRay, size_t InBounces, FHitRecord& InRecord, std::vector<LightComponent*> InLights) const
{
    bool objectHit = false;
    for (int i = 0; i < Hittables.size(); i++) 
	{
        // Cast a ray in object space for this hittable
        FRay objectSpaceRay = FRay(InRay.GetOrigin(), InRay.GetDirection());
        objectSpaceRay.ApplyTransform(Hittables[i]->InverseModelMatrix);
        bool bWasHitRecorded = Hittables[i]->Intersect(objectSpaceRay, .0001f, InRecord);

        if (bWasHitRecorded) {
            // Transform normal back to world space
            objectHit = true;
            InRecord.Normal = glm::normalize(glm::vec3(Hittables[i]->TransposeInverseModelMatrix * glm::vec4(InRecord.Normal, 0.0f)));
			InRecord.Material_ = Hittables[i]->Material_;
        }
    }

    if (objectHit) 
	{
		// Get rays
		glm::vec3 hitPosition = InRay.At(InRecord.Time);
		glm::vec3 eyeRay = glm::normalize(hitPosition - InRay.GetOrigin());
		glm::vec3 reflectedRay = glm::normalize(eyeRay - 2 * glm::dot(eyeRay, InRecord.Normal) * InRecord.Normal);

		// Get material properties
		glm::vec3 diffuseReflectance = InRecord.Material_.GetDiffuseColor();
		glm::vec3 specularReflectance = InRecord.Material_.GetSpecularColor();

		// Initialize lighting intensity variables
		glm::vec3 diffuseIntensity(0.f);
		glm::vec3 specularIntensity(0.f);
		glm::vec3 ambientIllumination(0.f);
		glm::vec3 overallIntensity(0.f);

		for (LightComponent* lightComp : InLights) {
			// Set up light variables and check for ambient light stength/color
			if (lightComp->GetLightPtr()->GetType() == ELightType::Ambient) {
				ambientIllumination = lightComp->GetLightPtr()->GetDiffuseColor();
				continue;
			};
			glm::vec3 directionToLight;
			glm::vec3 lightIntensity;
			float distanceToLight;
			GetIllumination(*lightComp, hitPosition, directionToLight, lightIntensity, distanceToLight);

			// Calculate diffuse shading component
			float clampedDiffuse = glm::clamp(glm::dot(directionToLight, InRecord.Normal), 0.f, std::numeric_limits<float>::max());
			diffuseIntensity += clampedDiffuse * lightIntensity * diffuseReflectance;

			// Calculate specular shading component
			float clampedSpecular = glm::clamp(glm::dot(directionToLight, reflectedRay), 0.f, std::numeric_limits<float>::max());
			float shininess = InRecord.Material_.GetShininess();
			specularIntensity += pow(clampedSpecular, shininess) * lightIntensity * specularReflectance;
		}
		
		// Calculate all shading components
		glm::vec3 ambientIntensity = ambientIllumination * diffuseReflectance;
		overallIntensity += ambientIntensity + diffuseIntensity + specularIntensity;

		return overallIntensity;
    }
    else 
	{
        return GetBackgroundColor(InRay.GetDirection());
    }
}

glm::vec3 FRayTracer::GetBackgroundColor(const glm::vec3& InDirection) const
{
	return Settings.BackgroundColor;
}

void FRayTracer::GetIllumination(const LightComponent& lightComponent, const glm::vec3& hitPos, glm::vec3& directionToLight, glm::vec3& intensity, float& distanceToLight) const
{
	auto lightPtr = lightComponent.GetLightPtr();
	if (lightPtr->GetType() == ELightType::Directional) {
		/*auto directional_light_ptr = static_cast<DirectionalLight*>(lightPtr);
		directionToLight = -directional_light_ptr->GetDirection();
		intensity = directional_light_ptr->GetDiffuseColor();
		distanceToLight = std::numeric_limits<float>::max();*/
	}
	else if (lightPtr->GetType() == ELightType::Point) {
		auto pointLightPtr = static_cast<PointLight*>(lightPtr);
		glm::vec3 pointLightPos = lightComponent.GetNodePtr()->GetTransform().GetPosition();
		glm::vec3 surfaceToLight = pointLightPos - hitPos;
		distanceToLight = glm::length(surfaceToLight);
		directionToLight = glm::normalize(surfaceToLight);
		intensity = pointLightPtr->GetDiffuseColor() / (pointLightPtr->GetAttenuation() * distanceToLight * distanceToLight);
	}
	else {
		throw std::runtime_error(
			"Unrecognized light type when computing "
			"illumination");
	}
}

}