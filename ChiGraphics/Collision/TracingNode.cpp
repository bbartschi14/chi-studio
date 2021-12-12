#include "TracingNode.h"
#include "ChiGraphics/Components/LightComponent.h"
#include "ChiGraphics/Lights/HittableLight.h"
#include "ChiGraphics/Meshes/PrimitiveFactory.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/Components/TracingComponent.h"
#include "Hittables/SphereHittable.h"
#include "ChiGraphics/Components/ShadingComponent.h"
#include "ChiGraphics/Shaders/SimpleShader.h"
#include "ChiGraphics/Components/MaterialComponent.h"

namespace CHISTUDIO {

TracingNode::TracingNode(std::string InName)
	: SceneNode(InName)
{
	float radius = 1.0f;
	auto sphereHittable = std::make_shared<SphereHittable>(radius, glm::vec3(0.0f));
	auto material = std::make_shared<Material>();
	CreateComponent<MaterialComponent>(material);

	auto tracingComponent = CreateComponent<TracingComponent>(std::move(sphereHittable), ETracingType::Sphere);

	auto hittableLight = std::make_shared<HittableLight>();
	auto lightComponent = CreateComponent<LightComponent>(hittableLight);

	// Construct debug visuals
	std::shared_ptr<SimpleShader> shader = std::make_shared<SimpleShader>();
	std::unique_ptr<VertexObject> mesh = PrimitiveFactory::CreateSphere(radius, 12, 12);

	std::unique_ptr<SceneNode> debugVisualNode = make_unique<SceneNode>("DebugVisualSphere");
	auto& renderingComp = debugVisualNode->CreateComponent<RenderingComponent>(std::move(mesh));
	renderingComp.SetPolygonMode(EPolygonMode::Fill);
	renderingComp.SetDrawMode(EDrawMode::Triangles);
	renderingComp.bIsDebugRender = true;

	debugVisualNode->CreateComponent<ShadingComponent>(shader);
	debugVisualNode->CreateComponent<MaterialComponent>(material);
	debugVisualNode->SetHierarchyVisible(false);
	AddChild(std::move(debugVisualNode));
}

void TracingNode::RefreshDebugVisual()
{
}

}