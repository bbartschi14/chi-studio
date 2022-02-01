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
#include "ChiGraphics/Lights/DirectionalLight.h"
#include "ChiGraphics/Lights/AmbientLight.h"
#include "ChiGraphics/Lights/HittableLight.h"
#include "core.h"
#include <chrono>
#include "ChiGraphics/Textures/ImageManager.h"
#include "ChiCore/ChiStudioApplication.h"
#include "external/src/oidn/include/OpenImageDenoise/oidn.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include "ChiGraphics/RNG.h"
#include <ctime>

namespace CHISTUDIO {

const float FIREFLY_CLAMP = 10.0f;

FRayTracer::FRayTracer(FRayTraceSettings InSettings)
	: Settings(InSettings)
{
}

static std::mutex RowsCompleteMutex;

void FRayTracer::RenderRow(size_t InY, std::vector<LightComponent*>* InLights, FTracingCamera* InTracingCamera, FImage* InOutputImage, FImage* InAlbedoImage, FImage* InNormalImage, int InRNGSeed)
{
	RNG rng = RNG(InRNGSeed);

	for (size_t x = 0; x < Settings.ImageSize.x; x++) {
		glm::vec3 pixelColor(0.f);
		glm::vec3 albedo(0.f);
		glm::vec3 normal(0.f);
		for (size_t sampleNumber = 0; sampleNumber < Settings.SamplesPerPixel; sampleNumber++)
		{
			double jitterX = Settings.SamplesPerPixel > 1 ? rng.Float() : 0.0;
			double jitterY = Settings.SamplesPerPixel > 1 ? rng.Float() : 0.0;

			// Set coords from [ -1, 1 ] for both x and y.
			float cameraX = ((float(x) + (float)jitterX) / (Settings.ImageSize.x - 1)) * 2 - 1;
			float cameraY = ((float(InY) + (float)jitterY) / (Settings.ImageSize.y - 1)) * 2 - 1;

			// Use camera coords to generate a ray into the scene
			FRay cameraToSceneRay = InTracingCamera->GenerateRay(glm::vec2(cameraX, cameraY), rng);
			glm::vec3 outAlbedo(-1.0f);
			glm::vec3 outNormal(0.0f);
			pixelColor += TraceRay(cameraToSceneRay, 0, *InLights, outAlbedo, outNormal, rng);
			albedo += outAlbedo;
			normal += outNormal;
		}
		float superSamplingScale = 1.0f / Settings.SamplesPerPixel;
		pixelColor *= superSamplingScale;
		albedo *= superSamplingScale;
		if (glm::length(normal) > 0.0000f)
		{
			normal = glm::normalize(normal);
		}

		InOutputImage->SetPixel(x, InY, pixelColor);
		InAlbedoImage->SetPixel(x, InY, albedo);
		InNormalImage->SetPixel(x, InY, normal);
	}
	RowsCompleteMutex.lock();
	RowsComplete++;
	std::cout << fmt::format("\rRendered: {:.2f}%", (float)RowsComplete / Settings.ImageSize.y * 100);// << std::endl;
	RowsCompleteMutex.unlock();
}

std::unique_ptr<FTexture> FRayTracer::Render(const Scene& InScene, const std::string& InOutputFile)
{
	RowsComplete = 0;
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
	auto outputImage = make_unique<FImage>(Settings.ImageSize.x, Settings.ImageSize.y);
	auto albedoImage = make_unique<FImage>(Settings.ImageSize.x, Settings.ImageSize.y);
	auto normalImage = make_unique<FImage>(Settings.ImageSize.x, Settings.ImageSize.y);

	std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();

	std::cout << "Initializing render threads" << std::endl;
	for (size_t y = 0; y < Settings.ImageSize.y; y++) 
	{
		Futures.push_back(std::async(std::launch::async, &FRayTracer::RenderRow, this, y, &lightComponents, tracingCamera.get(), outputImage.get(), albedoImage.get(), normalImage.get(), time(NULL) + y * 10000));
	}

	for (auto& future : Futures) {
		future.wait();
	}

	std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();

	std::cout << std::endl;
	std::cout << "Render Time = " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count() << "[ms], " 
		<< std::chrono::duration_cast<std::chrono::seconds>(endTime - beginTime).count() << "[s]" << std::endl;

	if (InOutputFile.size())
	{
		albedoImage->SavePNG(fmt::format("{}_albedo.png", InOutputFile));

		// Remap [-1, 1] normals in place to [0, 1]
		normalImage->RemapNormalData();
		normalImage->SavePNG(fmt::format("{}_normal.png", InOutputFile));

		if (Settings.UseIntelDenoise)
		{
			std::cout << "Denoising" << std::endl;
			oidn::DeviceRef device = oidn::newDevice();
			device.commit();
			// Create a filter for denoising a beauty (color) image using optional auxiliary images too
			oidn::FilterRef filter = device.newFilter("RT"); // generic ray tracing filter
			std::vector<float> dataBuffer = outputImage->ToFloatData();
			std::vector<float> albedoBuffer = albedoImage->ToFloatData();
			std::vector<float> normalBuffer = normalImage->ToFloatData();
			filter.setImage("color", dataBuffer.data(), oidn::Format::Float3, Settings.ImageSize.x, Settings.ImageSize.y); // beauty
			filter.setImage("albedo", albedoBuffer.data(), oidn::Format::Float3, Settings.ImageSize.x, Settings.ImageSize.y); // auxiliary - albedo map
			filter.setImage("normal", normalBuffer.data(), oidn::Format::Float3, Settings.ImageSize.x, Settings.ImageSize.y); // auxiliary - normal map
			filter.setImage("output", dataBuffer.data(), oidn::Format::Float3, Settings.ImageSize.x, Settings.ImageSize.y); // In-place on the beauty image
			filter.set("hdr", true); // beauty image is HDR
			filter.commit();

			// Filter the image
			filter.execute();

			// Check for errors
			const char* errorMessage;
			if (device.getError(errorMessage) != oidn::Error::None)
				std::cout << "Error: " << errorMessage << std::endl;

			outputImage->SetFloatData(dataBuffer, true);
		}

		if (Settings.UseCompositingNodes)
		{
			auto modifiedImagePtr = FImage::MakeImageCopy(outputImage.get());
			ChiStudioApplication* chiStudioApp = static_cast<ChiStudioApplication*>(InScene.GetAppRef());
			WImageCompositor* imageCompositingWidget = chiStudioApp->GetImageCompositingWidgetPtr();
			imageCompositingWidget->ApplyModifiersToImage(modifiedImagePtr.get());
			modifiedImagePtr->SavePNG(fmt::format("{}.png", InOutputFile));
		}
		else
		{
			outputImage->SavePNG(fmt::format("{}.png", InOutputFile));
		}
	}

	// Send pixel data to output texture for viewing
	OutputTexture->UpdateImage(*outputImage);
	ImageManager::GetInstance().SetRenderResult(std::move(outputImage));

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
	std::cout << "Building hittable data" << std::endl;

	auto& root = InScene.GetRootNode();
	std::vector<RenderingComponent*> renderingComps = root.GetComponentPtrsInChildren<RenderingComponent>();
	std::vector<TracingComponent*> tracingComps = root.GetComponentPtrsInChildren<TracingComponent>();

	for (RenderingComponent* renderingComp : renderingComps)
	{
		if (!renderingComp->bIsDebugRender)
		{
			VertexObject* vertexObject = renderingComp->GetVertexObjectPtr();
			if (vertexObject->GetPositions().size() == 0) continue;
			std::cout << "Building hittable for " << renderingComp->GetNodePtr()->GetNodeName() << std::endl;
			std::shared_ptr<MeshHittable> hittable = std::make_shared<MeshHittable>(vertexObject->GetPositions(), vertexObject->GetNormals(), vertexObject->GetIndices(), vertexObject->GetTexCoords());

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
			//std::cout << "Added" << std::endl;
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

glm::dvec3 FRayTracer::TraceRay(const FRay& InRay, size_t InBounces, std::vector<LightComponent*> InLights, glm::vec3& OutAlbedo, glm::vec3& OutNormal, RNG& InRNG)
{
	FHitRecord record;
    bool objectHit = GetClosestObjectHit(InRay, record, nullptr);

	if (objectHit) 
	{
		// Record albedo of first hit. Initial value is set to negative
		if (OutAlbedo.x < 0.0f)
		{
			OutAlbedo = record.Material_.SampleAlbedo(record.UV);
		}

		// Record normal of first hit. Initial value is set to length = 0.0f
		if (glm::length(OutNormal) < 0.0000001f)
		{
			OutNormal = record.Normal;
		}

		// Get rays
		glm::dvec3 hitPosition = InRay.At(record.Time);
		glm::dvec3 eyeRay = glm::normalize(glm::dvec3(InRay.GetOrigin()) - hitPosition);

		// Initialize color from emission first
		glm::dvec3 overallIntensity = (double)record.Material_.SampleEmittance(record.UV) * record.Material_.SampleAlbedo(record.UV);

		for (LightComponent* lightComp : InLights) {

			if (!lightComp->GetLightPtr()->IsLightEnabled()) continue;

			// Set up light variables and check for ambient light strength/Color
			if (lightComp->GetLightPtr()->GetType() == ELightType::Ambient) {
				overallIntensity += glm::dvec3(lightComp->GetLightPtr()->GetDiffuseColor()) * record.Material_.SampleAlbedo(record.UV);
			}
			else
			{
				glm::dvec3 directionToLight;
				glm::dvec3 lightIntensity;
				double distanceToLight;
				GetIllumination(*lightComp, hitPosition, directionToLight, lightIntensity, distanceToLight, InRNG);

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
					glm::dvec3 illumination = record.Material_.EvaluateBSDF(record.Normal, eyeRay, directionToLight, record.UV, InRNG);
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

			if (record.Material_.SampleHemisphere(sampledRayDirection, rayProbability, record.Normal, eyeRay, record.UV, InRNG))
			{
				glm::dvec3 indirect = record.Material_.EvaluateBSDF(record.Normal, eyeRay, sampledRayDirection, record.UV, InRNG);

				FRay tracedRay = FRay(hitPosition, sampledRayDirection);
				glm::dvec3 traceResult = TraceRay(tracedRay, InBounces + 1, InLights, OutAlbedo, OutNormal, InRNG);
				glm::dvec3 term = indirect * traceResult;
				glm::dvec3 indirectIllumination = 1.0 / rayProbability * term * glm::abs(glm::dot(sampledRayDirection, glm::dvec3(record.Normal)));

				if (glm::isnan(indirectIllumination.x))
				{
					return overallIntensity;
				}
				else
				{
					overallIntensity.x += glm::min((float)indirectIllumination.x, FIREFLY_CLAMP);
					overallIntensity.y += glm::min((float)indirectIllumination.y, FIREFLY_CLAMP);
					overallIntensity.z += glm::min((float)indirectIllumination.z, FIREFLY_CLAMP);
				}
			}
			
		}

		return overallIntensity;
    }
    else 
	{
		glm::vec3 backgroundColor = GetBackgroundColor(InRay.GetDirection());
		// Record albedo of first hit
		if (OutAlbedo.x < 0.0f)
		{
			OutAlbedo = backgroundColor;
		}
		return backgroundColor;
    }
}

glm::vec3 FRayTracer::GetBackgroundColor(const glm::vec3& InDirection) const
{
	if (Settings.HDRI != nullptr && Settings.UseHDRI)
	{
		return Settings.HDRI->SampleHDRI(InDirection) * Settings.HDRIStrength;
	}
	return Settings.BackgroundColor;
}

void FRayTracer::GetIllumination(const LightComponent& lightComponent, const glm::dvec3& hitPos, glm::dvec3& directionToLight, glm::dvec3& intensity, double& distanceToLight, RNG& InRNG)
{
	auto lightPtr = lightComponent.GetLightPtr();
	if (lightPtr->GetType() == ELightType::Directional) 
	{
		auto directionalLightPtr = static_cast<DirectionalLight*>(lightPtr);
		glm::vec3 direction = glm::mat4_cast(lightComponent.GetNodePtr()->GetTransform().GetRotation()) * glm::vec4(directionalLightPtr->BaseDirection, 0.0f);
		distanceToLight = 200000.0f;
		directionToLight = direction * -1.0f;
		intensity = directionalLightPtr->GetDiffuseColor() * directionalLightPtr->GetIntensity();
	}
	else if (lightPtr->GetType() == ELightType::Point) 
	{
		//https://developer.blender.org/diffusion/C/browse/master/src/kernel/light/light.h Reference blender's light sampling code for point light with radius
		auto pointLightPtr = static_cast<PointLight*>(lightPtr);
		glm::dvec3 center = lightComponent.GetNodePtr()->GetTransform().GetWorldPosition();
		float radius = pointLightPtr->GetRadius();
		float pdf = 1.0f;
		glm::vec3 normalOnLight = glm::normalize(hitPos - center);
		float inverseArea = 1.0f; // Default to 1.0f
		if (radius > 0.0f)
		{
			inverseArea = 1.0f / (kPi * radius * radius);
			center += DiskLightSample(normalOnLight, InRNG.Float(), InRNG.Float()) * radius;
			pdf = 1.0f / (kPi * radius * radius);
		}

		directionToLight = glm::normalize(center - hitPos);
		distanceToLight = glm::length(center - hitPos);

		// Calculate lamp light pdf
		float cosPi = glm::dot(normalOnLight, -(glm::vec3)directionToLight);
		pdf *= cosPi <= 0.0f ? 0.0f : ((float)distanceToLight * (float)distanceToLight / cosPi);
		float evalFactor = 1.0f / kPi * 0.25f * inverseArea;
		if (pdf > 0.0f)
		{
			intensity = (pointLightPtr->GetDiffuseColor() * pointLightPtr->GetIntensity()) * evalFactor / pdf;
		}
		else
		{
			intensity = glm::vec3( 0.0f );
		}
	}
	else if (lightPtr->GetType() == ELightType::Hittable) 
	{
		auto hittableLightPtr = static_cast<HittableLight*>(lightPtr);
		glm::vec3 outPosition;
		glm::vec3 outNormal;
		glm::vec3 transformedHitPosition = hittableLightPtr->GetHittable()->InverseModelMatrix * glm::vec4(hitPos, 1.0f);
		float outProbability = hittableLightPtr->GetHittable()->Sample(transformedHitPosition, outPosition, outNormal, InRNG);

		// Transform normal and pos back to world space
		outNormal = glm::normalize(glm::vec3(hittableLightPtr->GetHittable()->TransposeInverseModelMatrix * glm::vec4(outNormal, 0.0f)));
		outPosition = glm::vec3(hittableLightPtr->GetHittable()->ModelMatrix * glm::vec4(outPosition, 1.0f));

		glm::vec3 displacement = (glm::dvec3)outPosition - hitPos;
		distanceToLight = glm::length(displacement);
		float cosine = glm::max(glm::dot(-displacement, outNormal), 0.0f) / (float)distanceToLight;
		float surfaceArea = glm::max(cosine, 0.0f) / (float)(distanceToLight * distanceToLight);

		// TODO: Change GetAlbedo and GetEmittance to use material sample functions. Needs to get UVs from Hittable->Sample
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
			bool bWasHitRecorded = Hittables[i]->Intersect(objectSpaceRay, .00001f, InRecord, Hittables[i]->Material_);

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