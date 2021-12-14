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
	Camera = camera.get();
	// Construct debug visuals
	std::shared_ptr<SimpleShader> shader = std::make_shared<SimpleShader>();
	FDefaultObjectParams params;
	std::shared_ptr<VertexObject> mesh = std::make_shared<VertexObject>(EDefaultObject::Debug, params);
	DebugMesh = mesh.get();
	mesh->UpdatePositions(std::move(GetDebugPositions()));

	CameraDebugVisualNode = make_unique<SceneNode>("DebugVisualCamera");
	auto& renderingComp = CameraDebugVisualNode->CreateComponent<RenderingComponent>(mesh);
	renderingComp.SetDrawMode(EDrawMode::Lines);
	renderingComp.bRenderSolid = false;
	renderingComp.bIsDebugRender = true;
	auto material = std::make_shared<Material>();
	material->SetAlbedo(glm::vec3(0.0f, 0.0f, 0.0f));
	CameraDebugVisualNode->CreateComponent<ShadingComponent>(shader);
	CameraDebugVisualNode->CreateComponent<MaterialComponent>(material);
	CameraDebugVisualNode->SetHierarchyVisible(false);
	AddChild(std::move(CameraDebugVisualNode));
	AddComponent(std::move(camera));
}

void TracingCameraNode::RefreshDebugVisual()
{
	DebugMesh->UpdatePositions(std::move(GetDebugPositions()));
}

std::unique_ptr<FPositionArray> TracingCameraNode::GetDebugPositions()
{
	auto vertexPositions = make_unique<FPositionArray>();
	float length = 2.0f;
	glm::vec3 forwardVector = GetTransform().GetWorldForward();
	glm::vec3 upVector = GetTransform().GetWorldUp();
	glm::vec3 rightVector = GetTransform().GetWorldRight();

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

	// Draw focus distance visual
	glm::vec3 focusPoint = forwardVector * Camera->FocusDistance;
	vertexPositions->push_back(glm::vec3(0.0f));
	vertexPositions->push_back(focusPoint);

	vertexPositions->push_back(focusPoint + upVector * 1.0f);
	vertexPositions->push_back(focusPoint - upVector * 1.0f);

	vertexPositions->push_back(focusPoint + rightVector * 1.0f);
	vertexPositions->push_back(focusPoint - rightVector * 1.0f);
	return std::move(vertexPositions);
}

std::unique_ptr<FTracingCamera> TracingCameraNode::GetTracingCamera(glm::ivec2 InRenderSize) const
{
	FCameraSpec cameraSpec;
	cameraSpec.Center = GetTransform().GetWorldPosition();
	cameraSpec.Direction = glm::vec3(GetTransform().GetLocalToWorldMatrix() * glm::vec4(GetTransform().GetWorldForward(), 0.0f));
	cameraSpec.Up = glm::vec3(GetTransform().GetLocalToWorldMatrix() * glm::vec4(GetTransform().GetWorldUp(), 0.0f));
	cameraSpec.FOV_Degrees = GetComponentPtr<CameraComponent>()->GetFOV();
	cameraSpec.AspectRatio = (float)InRenderSize.y / (float)InRenderSize.x;
	cameraSpec.FocusDistance = GetComponentPtr<CameraComponent>()->FocusDistance;
	cameraSpec.Aperture = GetComponentPtr<CameraComponent>()->Aperture;
	std::unique_ptr<FTracingCamera> tracingCamera = make_unique<FTracingCamera>(cameraSpec);
	return tracingCamera;
}

}