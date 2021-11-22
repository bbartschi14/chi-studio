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
	BuildHittableData(InScene);

	FImage outputImage(Settings.ImageSize.x, Settings.ImageSize.y);

	float cameraX = ((float(Settings.ImageSize.x/2)) / (Settings.ImageSize.x - 1)) * 2 - 1;
	float cameraY = ((float(Settings.ImageSize.y/2 + 5)) / (Settings.ImageSize.y - 1)) * 2 - 1;

	// Use camera coords to generate a ray into the scene
	//FRay cameraToSceneRay = tracingCamera->GenerateRay(glm::vec2(cameraX, cameraY));
	//std::cout << std::endl;

	//glm::vec3 color = TraceRay(cameraToSceneRay, 0, lightComponents);
	//std::cout << "Final Color: " << glm::to_string(color) << std::endl;

	//std::cout << fmt::format("Nan count: {}", debugNANCount) << std::endl;
	//std::cout << fmt::format("Avg Indirect: {}", glm::to_string(debugAverageIndirect / (float)debugIndirectCount)) << std::endl;

	//return OutputTexture;

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
				//std::cout << "Final Color: " << glm::to_string(pixelColor) << std::endl;
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
	std::vector<LightComponent*> lightComps = root.GetComponentPtrsInChildren<LightComponent>();

	return lightComps;
}

void FRayTracer::BuildHittableData(const Scene& InScene)
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

			Hittables.emplace_back(std::move(hittable));
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

		Hittables.emplace_back(std::move(hittable));
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
    bool objectHit = GetClosestObjectHit(InRay, record);

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
				bool wasShadowObjectHit = GetClosestObjectHit(shadowRay, shadowRecord);
				double distanceToHit = glm::length((double)shadowRecord.Time * directionToLight);
				if (!wasShadowObjectHit || distanceToHit > distanceToLight)
				{
					// No object casting a shadow
					glm::dvec3 illumination = record.Material_.EvaluateBSDF(record.Normal, eyeRay, directionToLight);
					//std::cout << glm::to_string(illumination) << std::endl;
					overallIntensity += illumination * glm::dvec3(lightComp->GetLightPtr()->GetDiffuseColor()) * glm::dot(directionToLight, glm::dvec3(record.Normal));
				}
			}
		}

		if (InBounces < Settings.MaxBounces)
		{
			// Let's trace!
			glm::dvec3 sampledRayDirection;
			double rayProbability;
			//std::cout << "Hemisphere" << std::endl;
			if (record.Material_.SampleHemisphere(sampledRayDirection, rayProbability, record.Normal, eyeRay))
			{
				//std::cout << "Indirect BSDF sample" << std::endl;
				//std::cout.precision(17);
				glm::dvec3 indirect = record.Material_.EvaluateBSDF(record.Normal, eyeRay, sampledRayDirection);
				//std::cout << "Indirect: " << glm::to_string(indirect) << std::endl;

				FRay tracedRay = FRay(hitPosition, sampledRayDirection);
				glm::dvec3 traceResult = TraceRay(tracedRay, InBounces + 1, InLights);
				//std::cout << "Trace Result: " << glm::to_string(traceResult) << std::endl;

				glm::dvec3 term = indirect * traceResult;
				//std::cout << "1/pdf: " << std::fixed << 1.0 / rayProbability << std::endl;
				//std::cout << "Term: " << glm::to_string(term) << std::endl;
				//std::cout << "Dot: " << glm::abs(glm::dot(sampledRayDirection, glm::dvec3(record.Normal))) << std::endl;

				glm::dvec3 indirectIllumination = 1.0 / rayProbability * term * glm::abs(glm::dot(sampledRayDirection, glm::dvec3(record.Normal)));
				//std::cout << "Indirect Total" << glm::to_string(indirectIllumination) << std::endl;

				if (glm::isnan(indirectIllumination.x)) debugNANCount++;
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
		//std::cout << "Background" << std::endl;
        return GetBackgroundColor(InRay.GetDirection());
    }
}

glm::vec3 FRayTracer::GetBackgroundColor(const glm::vec3& InDirection) const
{
	return Settings.BackgroundColor;
}

void FRayTracer::GetIllumination(const LightComponent& lightComponent, const glm::dvec3& hitPos, glm::dvec3& directionToLight, glm::dvec3& intensity, double& distanceToLight)
{
	auto lightPtr = lightComponent.GetLightPtr();
	if (lightPtr->GetType() == ELightType::Directional) {
		/*auto directional_light_ptr = static_cast<DirectionalLight*>(lightPtr);
		directionToLight = -directional_light_ptr->GetDirection();
		intensity = directional_light_ptr->GetDiffusepixelColor();
		distanceToLight = std::numeric_limits<float>::max();*/
	}
	else if (lightPtr->GetType() == ELightType::Point) {
		auto pointLightPtr = static_cast<PointLight*>(lightPtr);
		glm::dvec3 pointLightPos = lightComponent.GetNodePtr()->GetTransform().GetPosition();
		glm::dvec3 surfaceToLight = pointLightPos - hitPos;
		distanceToLight = glm::length(surfaceToLight);
		directionToLight = glm::normalize(surfaceToLight);
	}
	else {
		throw std::runtime_error(
			"Unrecognized light type when computing "
			"illumination");
	}
}

bool FRayTracer::GetClosestObjectHit(const FRay& InRay, FHitRecord& InRecord) const
{
	bool objectHit = false;
	for (int i = 0; i < Hittables.size(); i++)
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
	return objectHit;
}

}