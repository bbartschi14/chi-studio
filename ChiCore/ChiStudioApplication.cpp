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

#include "glm/gtx/string_cast.hpp"

namespace CHISTUDIO {

ChiStudioApplication::ChiStudioApplication(const std::string& InAppName, glm::ivec2 InWindowSize)
	: Application(InAppName, InWindowSize)
{
	HierarchyWidget = make_unique<WHierarchy>();	
	ObjectPropertiesWidget = make_unique<WObjectProperties>();
	EditModeWidget = make_unique<WEditMode>();
}

void ChiStudioApplication::SetupScene()
{
	SceneNode& root = Scene_->GetRootNode();

	// Setup Camera
	std::unique_ptr<ArcBallCameraNode> cameraNode = make_unique<ArcBallCameraNode>(50.0f, 1.0f, 10.0f);
	cameraNode->GetTransform().SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	cameraNode->GetTransform().SetRotation(glm::vec3(0.0f, 1.0f, 0.0f), kPi / 2);
	cameraNode->Calibrate();
	Scene_->ActivateCamera(cameraNode->GetComponentPtr<CameraComponent>());
	root.AddChild(std::move(cameraNode));

	// Ambient Light
	//std::shared_ptr<AmbientLight> ambientLightPtr = std::make_shared<AmbientLight>();
	//ambientLightPtr->SetAmbientColor(glm::vec3(0.4f));
	//root.CreateComponent<LightComponent>(ambientLightPtr); // Add as a component to the root node
	
	// Point Light Node
	std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>();
	pointLight->SetDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
	pointLight->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
	pointLight->SetAttenuation(glm::vec3(1.0f, 0.09f, 0.032f));

	std::unique_ptr<SceneNode> pointLightNode = make_unique<SceneNode>("Point Light");
	pointLightNode->CreateComponent<LightComponent>(pointLight);
	pointLightNode->GetTransform().SetPosition(glm::vec3(0.0f, 4.0f, 5.f));
	root.AddChild(std::move(pointLightNode));

	// Setup Teapot
	std::shared_ptr<PhongShader> cubeShader = std::make_shared<PhongShader>();
	//std::shared_ptr<VertexObject> teapotMesh = MeshLoader::Import("teapot.obj").VertexObj;
	std::shared_ptr<VertexObject> cubeMesh = std::make_shared<VertexObject>();
	//if (teapotMesh == nullptr) {
	//	return;
	//}

	auto cubeNode = make_unique<SceneNode>("Cube");
	cubeNode->CreateComponent<ShadingComponent>(cubeShader);
	cubeNode->CreateComponent<RenderingComponent>(cubeMesh);
	cubeNode->GetTransform().SetPosition(glm::vec3(0.f, 0.f, 0.f));
	root.AddChild(std::move(cubeNode));
	
	auto cubeNode2 = make_unique<SceneNode>("Cube2");
	cubeNode2->CreateComponent<ShadingComponent>(cubeShader);
	std::shared_ptr<VertexObject> cubeMesh2 = std::make_shared<VertexObject>();
	cubeNode2->CreateComponent<RenderingComponent>(cubeMesh2);
	cubeNode2->GetTransform().SetPosition(glm::vec3(4.f, 0.f, 4.f));
	root.AddChild(std::move(cubeNode2));
	
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
	Renderer_->Render(*Scene_);

	HierarchyWidget->Render(*this);
	ObjectPropertiesWidget->Render(*this);
	EditModeWidget->Render(*this);
}

}
