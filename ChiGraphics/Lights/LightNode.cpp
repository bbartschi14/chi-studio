#include "AmbientLightNode.h"
#include "ChiGraphics/Components/LightComponent.h"
#include "ChiGraphics/Lights/AmbientLight.h"
#include "ChiGraphics/Meshes/PrimitiveFactory.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/Components/ShadingComponent.h"
#include "ChiGraphics/Shaders/SimpleShader.h"
#include "ChiGraphics/Components/MaterialComponent.h"

namespace CHISTUDIO {

AmbientLightNode::AmbientLightNode(std::string InName)
	: SceneNode(InName)
{
	auto ambientLight = std::make_shared<AmbientLight>();
	ambientLight->SetDiffuseColor(glm::vec3(0.1f, 0.1f, 0.1f));

	auto lightComponent = CreateComponent<LightComponent>(ambientLight);

	// Construct debug visuals
	std::shared_ptr<SimpleShader> shader = std::make_shared<SimpleShader>();
	std::unique_ptr<VertexObject> mesh = PrimitiveFactory::CreateSphere(.25f, 8, 8);

	std::unique_ptr<SceneNode> debugVisualNode = make_unique<SceneNode>("DebugVisualLight");
	auto& renderingComp = debugVisualNode->CreateComponent<RenderingComponent>(std::move(mesh));
	renderingComp.SetPolygonMode(EPolygonMode::Fill);
	renderingComp.SetDrawMode(EDrawMode::Triangles);
	renderingComp.bIsDebugRender = true;
	auto material = std::make_shared<Material>();
	if (auto lightComp = GetComponentPtr<LightComponent>())
	{
		material->SetAlbedo(lightComp->GetLightPtr()->GetDiffuseColor());
	}
	debugVisualNode->CreateComponent<ShadingComponent>(shader);
	debugVisualNode->CreateComponent<MaterialComponent>(material);
	debugVisualNode->SetHierarchyVisible(false);
	AddChild(std::move(debugVisualNode));
}

void AmbientLightNode::RefreshDebugVisual()
{
}

}