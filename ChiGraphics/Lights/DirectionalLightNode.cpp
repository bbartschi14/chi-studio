#include "DirectionalLightNode.h"
#include "ChiGraphics/Components/LightComponent.h"
#include "ChiGraphics/Lights/DirectionalLight.h"
#include "ChiGraphics/Meshes/PrimitiveFactory.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/Components/ShadingComponent.h"
#include "ChiGraphics/Shaders/SimpleShader.h"
#include "ChiGraphics/Components/MaterialComponent.h"

namespace CHISTUDIO {

DirectionalLightNode::DirectionalLightNode(std::string InName)
	: SceneNode(InName), BaseDirection(glm::vec3(1.0f, 0.0f, 0.0f))
{
	auto light = std::make_shared<DirectionalLight>();
	light->SetDiffuseColor(glm::vec3(1.0f, 1.0f, 1.0f));
	light->BaseDirection = BaseDirection;

	auto lightComponent = CreateComponent<LightComponent>(light);

	// Construct debug visuals
	std::shared_ptr<SimpleShader> shader = std::make_shared<SimpleShader>();
	FDefaultObjectParams params;
	std::shared_ptr<VertexObject> mesh = std::make_shared<VertexObject>(EDefaultObject::Debug, params);
	auto vertexPositions = make_unique<FPositionArray>();
	vertexPositions->push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	vertexPositions->push_back(BaseDirection);
	mesh->UpdatePositions(std::move(vertexPositions));

	std::unique_ptr<SceneNode> debugVisualNode = make_unique<SceneNode>("DebugVisualLight");
	auto& renderingComp = debugVisualNode->CreateComponent<RenderingComponent>(std::move(mesh));
	renderingComp.SetPolygonMode(EPolygonMode::Wireframe);
	renderingComp.SetDrawMode(EDrawMode::Lines);
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

void DirectionalLightNode::RefreshDebugVisual()
{
}

}