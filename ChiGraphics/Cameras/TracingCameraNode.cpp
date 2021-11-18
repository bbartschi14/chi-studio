#include "TracingCameraNode.h"
#include "ChiGraphics/Components/CameraComponent.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/Components/ShadingComponent.h"
#include "ChiGraphics/Shaders/SimpleShader.h"
#include "ChiGraphics/Components/MaterialComponent.h"
#include "ChiGraphics/RayTracing/FTracingCamera.h"

namespace CHISTUDIO {

TracingCameraNode::TracingCameraNode(std::string InName, float InFOV, float InAspectRatio)
	: SceneNode(InName)
{
	auto camera = make_unique<CameraComponent>(InFOV, InAspectRatio, 0.1f, 10000.f);
	AddComponent(std::move(camera));

	// Construct debug visuals
	std::shared_ptr<SimpleShader> shader = std::make_shared<SimpleShader>();
	std::shared_ptr<VertexObject> mesh = std::make_shared<VertexObject>(EDefaultObject::Debug);

	auto vertexPositions = make_unique<FPositionArray>();
	float length = 2.0f;
	glm::vec3 forwardVector = GetTransform().GetForwardDirection();
	glm::vec3 upVector = GetTransform().GetUpDirection();
	glm::vec3 rightVector = GetTransform().GetRightDirection();

	glm::vec3 rectTopRight = forwardVector * length + rightVector * length + upVector * length;
	glm::vec3 rectTopLeft = forwardVector * length + rightVector * -length + upVector * length;
	glm::vec3 rectBottomRight = forwardVector * length + rightVector * length + upVector * -length;
	glm::vec3 rectBottomLeft = forwardVector * length + rightVector * -length + upVector * -length;

	vertexPositions->push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	vertexPositions->push_back(rectTopRight);

	vertexPositions->push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	vertexPositions->push_back(rectTopLeft);

	vertexPositions->push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	vertexPositions->push_back(rectBottomRight);

	vertexPositions->push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	vertexPositions->push_back(rectBottomLeft);

	vertexPositions->push_back(rectTopRight);
	vertexPositions->push_back(rectTopLeft);

	vertexPositions->push_back(rectTopLeft);
	vertexPositions->push_back(rectBottomLeft);

	vertexPositions->push_back(rectBottomLeft);
	vertexPositions->push_back(rectBottomRight);

	vertexPositions->push_back(rectBottomRight);
	vertexPositions->push_back(rectTopRight);

	mesh->UpdatePositions(std::move(vertexPositions));

	std::unique_ptr<SceneNode> cameraDebugVisualNode = make_unique<SceneNode>("DebugVisualCamera");
	auto& renderingComp = cameraDebugVisualNode->CreateComponent<RenderingComponent>(mesh);
	renderingComp.SetDrawMode(EDrawMode::Lines);
	renderingComp.bRenderSolid = false;
	renderingComp.bIsDebugRender = true;
	auto material = std::make_shared<Material>();
	material->SetDiffuseColor(glm::vec3(0.0f, 0.0f, 0.0f));
	cameraDebugVisualNode->CreateComponent<ShadingComponent>(shader);
	cameraDebugVisualNode->CreateComponent<MaterialComponent>(material);
	cameraDebugVisualNode->SetHierarchyVisible(false);
	AddChild(std::move(cameraDebugVisualNode));
}

void TracingCameraNode::RefreshDebugVisual()
{
}

std::unique_ptr<FTracingCamera> TracingCameraNode::GetTracingCamera(glm::ivec2 InRenderSize) const
{
	FCameraSpec cameraSpec;
	cameraSpec.Center = GetTransform().GetWorldPosition();
	cameraSpec.Direction = GetTransform().GetForwardDirection();
	cameraSpec.Up = GetTransform().GetUpDirection();
	cameraSpec.FOV_Degrees = GetComponentPtr<CameraComponent>()->GetFOV();
	cameraSpec.AspectRatio = (float)InRenderSize.y / (float)InRenderSize.x;
	std::unique_ptr<FTracingCamera> tracingCamera = make_unique<FTracingCamera>(cameraSpec);
	return tracingCamera;
}

}