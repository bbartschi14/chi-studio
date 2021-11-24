#include "RayTracer.h"
#include "ChiGraphics/GL_Wrapper/FTexture.h"
#include "ChiGraphics/Scene.h"
#include "ChiGraphics/Collision/Hittables/MeshHittable.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/Components/LightComponent.h"
#include "ChiGraphics/Components/MaterialComponent.h"
#include "ChiGraphics/Components/TracingComponent.h"
#include "ChiGraphics/RayTracing/FTracingCamera.h"
#include "ChiGraphics/Cameras/TracingCameraNode.h"
#include "ChiGraphics/GL_Wrapper/FTexture.h"
#include "ChiGraphics/Utilities.h"
#include "ChiGraphics/Lights/PointLight.h"
#include "ChiGraphics/Lights/AmbientLight.h"
#include "ChiGraphics/Lights/HittableLight.h"
#include "core.h"

namespace CHISTUDIO {

FRayTracer::FRayTracer(FRayTraceSettings InSettings)
	: Settings(InSettings)
{
}

std::unique_ptr<FTexture> FRayTracer::Render(const Scene& InScene, const std::string& InOutputFile)
{
	debugNANCount = 0;
	debugIndirectCount = 0;
	debugAverageIndirect = glm::vec3(0.f);

	auto OutputTexture = make_unique<FTexture>();
	OutputTexture->Reserve(GL_RGB, Settings.ImageSize.x, Settings.ImageSize.y, GL_RGBA, GL_UNSIGNED_BYTE);

	std::unique_ptr<FTracingCamera> tracingCamera = GetFirstTracingCamera(InScene);
	if (tracingCamera == nullptr)
	{
		std::cout << "No tracing camera" << std::endl;
		return OutputTexture;
	}

	auto lightComponents = GetLightComponents(InScene);
	BuildHittableData(InScene, lightComponents);
	std::cout << "Size: " << lightComponents.size() << std::endl;
	FImage outputImage(Settings.ImageSize.x, Settings.ImageSize.y);

	for (size_t y = 0; y < Settings.ImageSize.y; y++) {
		for (size_t x = 0; x < Settings.ImageSize.x; x++) {
			glm::vec3 pixelColor(0.f);

			for (size_t sampleNumber = 0; sampleNumber < Settings.SamplesPerPixel; sampleNumber++)
			{
				double jitterX = Settings.SamplesPerPixel > 1 ? RandomDouble() : 0.0;
				double jitterY = Settings.SamplesPerPixel > 1 ? RandomDouble() : 0.0;

				// Set coords from [ -1, 1 ] for both x and y.
				float cameraX = ((float(x) + (float)jitterX) / (Settings.ImageSize.x - 1)) * 2 - 1;
				float cameraY = ((float(y) + (float)jitterY) / (Settings.ImageSize.y - 1)) * 2 - 1;

				// Use camera coords to generate a ray into the scene
				FRay cameraToSceneRay = tracingCamera->GenerateRay(glm::vec2(cameraX, cameraY));
				pixelColor += TraceRay(cameraToSceneRay, 0, lightComponents);
			}
			float superSamplingScale = 1.0 / Settings.SamplesPerPixel;
			pixelColor *= superSamplingScale;
			glm::vec3 gammaCorrectedPixelColor = glm::vec3(sqrt(pixelColor.x), sqrt(pixelColor.y), sqrt(pixelColor.z));
			outputImage.SetPixel(x, y, gammaCorrectedPixelColor);
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
	std::vector<LightComponent*> lightComps;
	for (LightComponent* lightComp : root.GetComponentPtrsInChildren<LightComponent>())
	{
		if (lightComp->GetLightType() != ELightType::Hittable)
		{
			// Hittable lights are added in the BuildHittableData function
			lightComps.push_back(lightComp);
		}
	}
	return lightComps;
}

void FRayTracer::BuildHittableData(const Scene& InScene, std::vector<LightComponent*>& InLights)
{
	Hittables.clear();

	auto& root = InScene.GetRootNode();
	std::vector<RenderingComponent*> renderingComps = root.GetComponentPtrsInChildren<RenderingComponent>();
	std::vector<TracingComponent*> tracingComps = root.GetComponentPtrsInChildren<TracingComponent>();

	for (RenderingComponent* renderingComp : renderingComps)
	{
		if (!renderingComp->bIsDebugRender)
		{
			VertexObject* vertexObject = renderingComp->GetVertexObjectPtr();
			std::shared_ptr<MeshHittable> hittable = std::make_shared<MeshHittable>(vertexObject->GetPositions(), vertexObject->GetNormals(), vertexObject->GetIndices());

			hittable->ModelMatrix = renderingComp->GetNodePtr()->GetTransform().GetLocalToWorldMatrix();
			hittable->InverseModelMatrix = glm::inverse(hittable->ModelMatrix);
			hittable->TransposeInverseModelMatrix = glm::transpose(hittable->InverseModelMatrix);

			if (auto materialComp = renderingComp->GetNodePtr()->GetComponentPtr<MaterialComponent>())
			{
				hittable->Material_ = materialComp->GetMaterial();
			}
			else
			{
				hittable->Material_ = Material();
			}

			if (auto light = renderingComp->GetNodePtr()->GetComponentPtr<LightComponent>())
			{
				if (light->GetLightType() == ELightType::Hittable)
				{
					if (light->GetLightPtr()->IsLightEnabled() && hittable->Material_.GetEmittance() > 0.0f)
					{
						auto hittableLight = static_cast<HittableLight*>(light->GetLightPtr());
						hittableLight->SetHittable(hittable);
						InLights.emplace_back(light);
					}
				}
			}

			Hittables.emplace_back(hittable);
			std::cout << "Added" << std::endl;
		}

	}

	for (TracingComponent* tracingComp : tracingComps)
	{
		std::shared_ptr<IHittableBase> hittable = tracingComp->Hittable;

		hittable->ModelMatrix = tracingComp->GetNodePtr()->GetTransform().GetLocalToWorldMatrix();
		hittable->InverseModelMatrix = glm::inverse(hittable->ModelMatrix);
		hittable->TransposeInverseModelMatrix = glm::transpose(hittable->InverseModelMatrix);

		if (auto materialComp = tracingComp->GetNodePtr()->GetComponentPtr<MaterialComponent>())
		{
			hittable->Material_ = materialComp->GetMaterial();
		}
		else
		{
			hittable->Material_ = Material();
		}

		if (auto light = tracingComp->GetNodePtr()->GetComponentPtr<LightComponent>())
		{
			if (light->GetLightType() == ELightType::Hittable)
			{
				if (light->GetLightPtr()->IsLightEnabled() && hittable->Material_.GetEmittance() > 0.0f)
				{
					auto hittableLight = static_cast<HittableLight*>(light->GetLightPtr());
					hittableLight->SetHittable(hittable);
					InLights.emplace_back(light);
				}
			}
		}

		Hittables.emplace_back(hittable);
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

glm::dvec3 FRayTracer::TraceRay(const FRay& InRay, size_t InBounces, std::vector<LightComponent*> InLights)
{
	FHitRecord record;
    bool objectHit = GetClosestObjectHit(InRay, record, nullptr);

	if (objectHit) 
	{
		// Get rays
		glm::dvec3 hitPosition = InRay.At(record.Time);
		glm::dvec3 eyeRay = glm::normalize(glm::dvec3(InRay.GetOrigin()) - hitPosition);

		// Initialize color from emission first
		glm::dvec3 overallIntensity = (double)record.Material_.GetEmittance() * record.Material_.GetAlbedo();

		for (LightComponent* lightComp : InLights) {
			// Set up light variables and check for ambient light strength/Color
			if (lightComp->GetLightPtr()->GetType() == ELightType::Ambient) {
				overallIntensity += glm::dvec3(lightComp->GetLightPtr()->GetDiffuseColor()) * record.Material_.GetAlbedo();
			}
			else
			{
				glm::dvec3 directionToLight;
				glm::dvec3 lightIntensity;
				double distanceToLight;
				GetIllumination(*lightComp, hitPosition, directionToLight, lightIntensity, distanceToLight);

				FHitRecord shadowRecord;
				FRay shadowRay = FRay(hitPosition, directionToLight);

				// When using hittable lights, we pass it in as a mask to ignore
				std::shared_ptr<IHittableBase> toIgnore = nullptr;
				if (lightComp->GetLightType() == ELightType::Hittable) 
				{
					HittableLight* hittableLight = static_cast<HittableLight*>(lightComp->GetLightPtr());
					toIgnore = hittableLight->GetHittable();
				}

				bool wasShadowObjectHit = GetClosestObjectHit(shadowRay, shadowRecord, toIgnore);
				double distanceToHit = glm::length((double)shadowRecord.Time * directionToLight);
				if (!wasShadowObjectHit || distanceToHit > distanceToLight)
				{
					// No object casting a shadow
					glm::dvec3 illumination = record.Material_.EvaluateBSDF(record.Normal, eyeRay, directionToLight);
					overallIntensity += illumination * lightIntensity * glm::dot(directionToLight, glm::dvec3(record.Normal));
				}
				else
				{
					//std::cout << "In Shadow" << std::endl;
				}
			}
		}

		if (InBounces < Settings.MaxBounces)
		{
			// Let's trace!
			glm::dvec3 sampledRayDirection;
			double rayProbability;

			if (record.Material_.SampleHemisphere(sampledRayDirection, rayProbability, record.Normal, eyeRay))
			{
				glm::dvec3 indirect = record.Material_.EvaluateBSDF(record.Normal, eyeRay, sampledRayDirection);

				FRay tracedRay = FRay(hitPosition, sampledRayDirection);
				glm::dvec3 traceResult = TraceRay(tracedRay, InBounces + 1, InLights);
				glm::dvec3 term = indirect * traceResult;
				glm::dvec3 indirectIllumination = 1.0 / rayProbability * term * glm::abs(glm::dot(sampledRayDirection, glm::dvec3(record.Normal)));

				//std::cout << "Ray probability: " << rayProbability << std::endl;
				//std::cout << "Term: " << glm::to_string(term) << std::endl;
				//std::cout << "Dot: " << glm::abs(glm::dot(sampledRayDirection, glm::dvec3(record.Normal))) << std::endl;

				if (glm::isnan(indirectIllumination.x))
				{
					//std::cout << "NAN" << std::endl;
					return overallIntensity;
				}
				else
				{
					overallIntensity += indirectIllumination;
				}
				debugIndirectCount++;
				debugAverageIndirect += indirectIllumination;
			}
			
		}

		return overallIntensity;
    }
    else 
	{
        return GetBackgroundColor(InRay.GetDirection());
    }
}

glm::vec3 FRayTracer::GetBackgroundColor(const glm::vec3& InDirection) const
{
	if (Settings.HDRI != nullptr)
	{
		return Settings.HDRI->SampleHDRI(InDirection);
	}
	return Settings.BackgroundColor;
}

void FRayTracer::GetIllumination(const LightComponent& lightComponent, const glm::dvec3& hitPos, glm::dvec3& directionToLight, glm::dvec3& intensity, double& distanceToLight)
{
	auto lightPtr = lightComponent.GetLightPtr();
	if (lightPtr->GetType() == ELightType::Directional) 
	{
		// TODO : Implement directional light type
	}
	else if (lightPtr->GetType() == ELightType::Point) 
	{
		auto pointLightPtr = static_cast<PointLight*>(lightPtr);
		glm::dvec3 pointLightPos = lightComponent.GetNodePtr()->GetTransform().GetWorldPosition();
		glm::dvec3 surfaceToLight = pointLightPos - hitPos;
		distanceToLight = glm::length(surfaceToLight);
		directionToLight = glm::normalize(surfaceToLight);
		intensity = pointLightPtr->GetDiffuseColor();
	}
	else if (lightPtr->GetType() == ELightType::Hittable) 
	{
		auto hittableLightPtr = static_cast<HittableLight*>(lightPtr);
		glm::vec3 outPosition;
		glm::vec3 outNormal;
		glm::vec3 transformedHitPosition = hittableLightPtr->GetHittable()->InverseModelMatrix * glm::vec4(hitPos, 1.0f);
		float outProbability = hittableLightPtr->GetHittable()->Sample(transformedHitPosition, outPosition, outNormal);

		// Transform normal and pos back to world space
		outNormal = glm::normalize(glm::vec3(hittableLightPtr->GetHittable()->TransposeInverseModelMatrix * glm::vec4(outNormal, 0.0f)));
		outPosition = glm::vec3(hittableLightPtr->GetHittable()->ModelMatrix * glm::vec4(outPosition, 1.0f));

		glm::vec3 displacement = (glm::dvec3)outPosition - hitPos;
		distanceToLight = glm::length(displacement);
		float cosine = glm::max(glm::dot(-displacement, outNormal), 0.0f) / distanceToLight;
		float surfaceArea = glm::max(cosine, 0.0f) / (distanceToLight * distanceToLight);

		intensity = (glm::vec3)hittableLightPtr->GetHittable()->Material_.GetAlbedo() * hittableLightPtr->GetHittable()->Material_.GetEmittance() * surfaceArea / outProbability;
		directionToLight = displacement / (float)distanceToLight;
	}
	else 
	{
		throw std::runtime_error(
			"Unrecognized light type when computing "
			"illumination");
	}
}

bool FRayTracer::GetClosestObjectHit(const FRay& InRay, FHitRecord& InRecord, std::shared_ptr<IHittableBase> InHittableToIgnore) const
{
	bool objectHit = false;
	for (int i = 0; i < Hittables.size(); i++)
	{
		if (Hittables[i] != InHittableToIgnore)
		{
			// Cast a ray in object space for this hittable
			FRay objectSpaceRay = FRay(InRay.GetOrigin(), InRay.GetDirection());
			objectSpaceRay.ApplyTransform(Hittables[i]->InverseModelMatrix);
			bool bWasHitRecorded = Hittables[i]->Intersect(objectSpaceRay, .00001f, InRecord);

			if (bWasHitRecorded) {
				// Transform normal back to world space
				objectHit = true;
				InRecord.Normal = glm::normalize(glm::vec3(Hittables[i]->TransposeInverseModelMatrix * glm::vec4(InRecord.Normal, 0.0f)));
				InRecord.Material_ = Hittables[i]->Material_;
			}
		}
	}

		
	return objectHit;
}

}