#include "ChiStudioApplication.h"
#include "ChiGraphics/Cameras/ArcBallCameraNode.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/Components/MaterialComponent.h"
#include "ChiGraphics/Components/CameraComponent.h"
#include "ChiGraphics/Components/ShadingComponent.h"
#include "ChiGraphics/Components/LightComponent.h"
#include "ChiGraphics/Lights/AmbientLight.h"
#include "ChiGraphics/Lights/PointLight.h"
#include "ChiGraphics/Shaders/PhongShader.h"
#include "ChiGraphics/Shaders/PointShader.h"
#include "ChiGraphics/Meshes/MeshLoader.h"
#include "ChiGraphics/XYZGridNode.h"

#include "glm/gtx/string_cast.hpp"

namespace CHISTUDIO {

ChiStudioApplication::ChiStudioApplication(const std::string& InAppName, glm::ivec2 InWindowSize)
	: Application(InAppName, InWindowSize)
{
	HierarchyWidget = make_unique<WHierarchy>();	
	ObjectPropertiesWidget = make_unique<WObjectProperties>();
	EditModeWidget = make_unique<WEditMode>();
	RenderingWidget = make_unique<WRendering>();
}

void ChiStudioApplication::SetupScene()
{
	SceneNode& root = Scene_->GetRootNode();

	// Grid
	std::unique_ptr<XYZGridNode> gridNode = make_unique<XYZGridNode>();
	gridNode->SetHierarchyVisible(false);
	root.AddChild(std::move(gridNode));

	// Setup Camera
	std::unique_ptr<ArcBallCameraNode> cameraNode = make_unique<ArcBallCameraNode>(50.0f, 1.0f, 10.0f);
	cameraNode->GetTransform().SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	cameraNode->GetTransform().SetRotation(glm::vec3(0.0f, 1.0f, 0.0f), kPi / 2);
	cameraNode->Calibrate();
	Scene_->ActivateCamera(cameraNode->GetComponentPtr<CameraComponent>());
	cameraNode->SetHierarchyVisible(false);

	root.AddChild(std::move(cameraNode));

	//CreatePrimitiveNode(EDefaultObject::Cube);
	//CreateTracingSphereNode();

	SceneNode* tracingCamera = CreateCamera();
	tracingCamera->GetTransform().SetPosition(glm::vec3(0.0f, 2.0f, 10.0f));
	//tracingCamera->GetTransform().SetRotation(glm::vec3(-45.0f, 45.0f, 0.0f));

	//SceneNode* tracingPointLight = CreatePointLight();
	//tracingPointLight->GetTransform().SetPosition(glm::vec3(0.0f, 5.0f, 5.0f));

	SceneNode* plane = CreatePrimitiveNode(EDefaultObject::Plane);
	plane->GetTransform().SetScale(glm::vec3(5.0f, 1.0f, 5.0f));
	plane->GetComponentPtr<MaterialComponent>()->GetMaterial().SetAlbedo(glm::vec3(.1f));

	SceneNode* teapot = CreateImportMeshNode("teapot.obj");

}

void ChiStudioApplication::DrawGUI()
{
	ImGui::ShowDemoWindow();

	// Create the docking environment
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	float minimumWindowSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 300.0f;
	ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	style.WindowMinSize.x = minimumWindowSizeX;

	ImGui::End();

	// Rendering scene and GUI.
	RenderingWidget->Render(*this);
	Renderer_->Render(*Scene_);

	HierarchyWidget->Render(*this);
	ObjectPropertiesWidget->Render(*this);
	EditModeWidget->Render(*this);
}

}
