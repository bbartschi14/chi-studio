#include "XYZGridNode.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/Components/ShadingComponent.h"
#include "ChiGraphics/Shaders/SimpleShader.h"
#include "ChiGraphics/Components/MaterialComponent.h"

namespace CHISTUDIO {

XYZGridNode::XYZGridNode() : SceneNode("XYZGrid")
{
	std::shared_ptr<SimpleShader> shader = std::make_shared<SimpleShader>();
	std::shared_ptr<VertexObject> mesh = std::make_shared<VertexObject>(EDefaultObject::Debug);

	auto vertexPositions = make_unique<FPositionArray>();
	float range = 1000.0f;

	vertexPositions->push_back(glm::vec3(-range, 0.0f, 0.0f));
	vertexPositions->push_back(glm::vec3(range, 0.0f, 0.0f));
	//vertexPositions->push_back(glm::vec3(0.0f, -range, 0.0f));
	//vertexPositions->push_back(glm::vec3(0.0f, range, 0.0f));
	//vertexPositions->push_back(glm::vec3(0.0f, 0.0f, -range));
	//vertexPositions->push_back(glm::vec3(0.0f, 0.0f, range));

	mesh->UpdatePositions(std::move(vertexPositions));

	std::unique_ptr<SceneNode> xAxisNode = make_unique<SceneNode>("X Axis");
	auto& xAxisRendering = xAxisNode->CreateComponent<RenderingComponent>(mesh);
	xAxisRendering.SetDrawMode(EDrawMode::Lines);
	xAxisRendering.bRenderSolid = false;
	xAxisRendering.bIsDebugRender = true;
	xAxisNode->GetTransform().SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	auto xMaterial = std::make_shared<Material>();
	xMaterial->SetAlbedo(glm::vec3(.6f, 0.15f, 0.0f));
	xAxisNode->CreateComponent<ShadingComponent>(shader);
	xAxisNode->CreateComponent<MaterialComponent>(xMaterial);
	AddChild(std::move(xAxisNode));

	/*std::unique_ptr<SceneNode> yAxisNode = make_unique<SceneNode>("Y Axis");
	auto& yAxisRendering = yAxisNode->CreateComponent<RenderingComponent>(mesh);
	yAxisRendering.SetDrawMode(EDrawMode::Lines);
	yAxisRendering.bRenderSolid = false;
	yAxisRendering.bIsDebugRender = true;
	yAxisNode->GetTransform().SetRotation(glm::vec3(0.0f, 0.0f, 90.0f));
	auto yMaterial = std::make_shared<Material>();
	yMaterial->SetDiffuseColor(glm::vec3(0.0f, 1.0f, 0.0f));
	yAxisNode->CreateComponent<ShadingComponent>(shader);
	yAxisNode->CreateComponent<MaterialComponent>(yMaterial);
	AddChild(std::move(yAxisNode));*/

	std::unique_ptr<SceneNode> zAxisNode = make_unique<SceneNode>("Z Axis");
	auto& zAxisRendering = zAxisNode->CreateComponent<RenderingComponent>(mesh);
	zAxisRendering.SetDrawMode(EDrawMode::Lines);
	zAxisRendering.bRenderSolid = false;
	zAxisRendering.bIsDebugRender = true;
	zAxisNode->GetTransform().SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
	auto zMaterial = std::make_shared<Material>();
	zMaterial->SetAlbedo(glm::vec3(0.15f, 0.0f, .6f));
	zAxisNode->CreateComponent<ShadingComponent>(shader);
	zAxisNode->CreateComponent<MaterialComponent>(zMaterial);
	AddChild(std::move(zAxisNode));
}

}