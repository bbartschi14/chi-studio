#include "Application.h"
#include <iostream>

#include "ChiGraphics/Utilities.h"
#include "ChiGraphics/InputManager.h"
#include "ChiGraphics/Collision/FRay.h"
#include "ChiGraphics/Cameras/ArcBallCameraNode.h"
#include "ChiGraphics/Cameras/TracingCameraNode.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/Components/MaterialComponent.h"
#include "ChiGraphics/Components/CameraComponent.h"
#include "ChiGraphics/Components/ShadingComponent.h"
#include "ChiGraphics/Components/LightComponent.h"
#include "ChiGraphics/Lights/AmbientLight.h"
#include "ChiGraphics/Lights/PointLight.h"
#include "ChiGraphics/Lights/LightNode.h"
#include "ChiGraphics/Lights/DirectionalLightNode.h"
#include "ChiGraphics/Lights/HittableLight.h"
#include "ChiGraphics/Lights/AmbientLightNode.h"
#include "ChiGraphics/Shaders/PhongShader.h"
#include "ChiGraphics/Shaders/PointShader.h"
#include "ChiGraphics/Meshes/MeshLoader.h"
#include "ChiGraphics/Collision/TracingNode.h"
#include "external/src/ImGuizmo/ImGuizmo.h"
#include "ChiGraphics/Collision/Hittables/MeshHittable.h"
#include "ChiGraphics/Materials/MaterialManager.h"
#include "ChiGraphics/Keyframing/KeyframeManager.h"

namespace CHISTUDIO {

	Application::Application(std::string InAppName, glm::ivec2 InWindowSize)
		: AppName(InAppName), WindowSize(InWindowSize), bIsPreviewingRenderCamera(false)
	{
		CurrentFilename = "";

		InitializeGLFW();
		InitializeGUI();

		CurrentSceneMode = ESceneMode::Object;
		EditModeSelectionType = EEditModeSelectionType::Vertex;
		Scene_ = make_unique<Scene>(make_unique<SceneNode>("Root"));
		Scene_->SetAppRef(this);
		Renderer_ = make_unique<Renderer>(*this);

		KeyframeManager::GetInstance().SetAppRef(this);
	}

	Application::~Application()
	{
		Scene_.release();
		Renderer_.release();
		DestroyGUI();
		glfwDestroyWindow(WindowHandle);
		glfwTerminate();
	}

	bool Application::IsFinished()
	{
		return glfwWindowShouldClose(WindowHandle);
	}

	void Application::Tick(double InDeltaTime, double InCurrentTime)
	{
		// Process window events.
		glfwPollEvents();
		UpdateGUI((float)InDeltaTime);

		// Logic update before rendering.
		if (bIsPreviewingRenderCamera)
		{
			SceneNode* firstTracingCamera = Scene_->GetTracingCameraNode();
			if (firstTracingCamera)
			{
				// Copy params
				CameraComponent* viewCamera = Scene_->GetActiveCameraPtr();
				CameraComponent* renderCamera = firstTracingCamera->GetComponentPtr<CameraComponent>();

				viewCamera->GetNodePtr()->GetTransform().SetMatrix4x4(renderCamera->GetNodePtr()->GetTransform().GetLocalToWorldMatrix());
				viewCamera->SetViewMatrix(glm::inverse(renderCamera->GetNodePtr()->GetTransform().GetLocalToWorldMatrix()));
				viewCamera->SetFOV(renderCamera->GetFOV());
			}
		}
		Scene_->Update(InDeltaTime);

		RenderGUI();

		glfwSwapBuffers(WindowHandle);
	}

	void Application::FramebufferSizeCallback(glm::ivec2 InWindowSize)
	{
		WindowSize = InWindowSize;
		GL_CHECK(glViewport(0, 0, WindowSize.x, WindowSize.y));
	}

	void Application::SelectNode(SceneNode* nodeToSelect, bool addToSelection)
	{
		if (!addToSelection || nodeToSelect == nullptr)
		{
			if (CurrentSceneMode == ESceneMode::Edit)
				SetSceneMode(ESceneMode::Object);

			DeselectAllNodes();
		}

		if (nodeToSelect)
		{
			//std::cout << "Selecting " << nodeToSelect->GetNodeName() << std::endl;
			SelectedNodes.emplace_back(nodeToSelect);
			nodeToSelect->SetSelected(true);

			if (SelectedNodes.size() > 1)
			{
				SetSceneMode(ESceneMode::Object);
			}
		}
	}

	void Application::DeselectNode(SceneNode* nodeToDeselect)
	{
	}

	void Application::DeselectAllNodes()
	{
		for (int i = 0; i < SelectedNodes.size(); i++)
		{
			SelectedNodes[i]->SetSelected(false);
		}
		SelectedNodes.clear();
	}

	void Application::SetSceneMode(ESceneMode InSceneMode)
	{
		CurrentSceneMode = InSceneMode;

		if (SelectedNodes.size() > 0)
		{
			if (auto renderingComp = SelectedNodes[0]->GetComponentPtr<RenderingComponent>())
			{
				if (CurrentSceneMode == ESceneMode::Edit)
				{
					renderingComp->OnEnterEditMode();
				}
				else
				{
					renderingComp->OnExitEditMode();
				}
			}
		}

	}

	void Application::SetEditModeSelectionType(EEditModeSelectionType InSelectionType)
	{
		// First deselect any no longer selectable prims
		if (SelectedNodes.size() == 1)
		{
			if (RenderingComponent* renderingComponent = SelectedNodes[0]->GetComponentPtr<RenderingComponent>())
			{
				bool bAreVerticesSelectable = AreVerticesSelectableInGivenEditMode(InSelectionType);
				bool bAreEdgesSelectable = AreEdgesSelectableInGivenEditMode(InSelectionType);
				bool bAreFacesSelectable = AreFacesSelectableInGivenEditMode(InSelectionType);

				VertexObject* vertexObject = renderingComponent->GetVertexObjectPtr();
				if (!bAreVerticesSelectable)
				{
					vertexObject->ClearSelectedVertices();
				}
				if (!bAreEdgesSelectable)
				{
					vertexObject->ClearSelectedEdges();
				}
				if (!bAreFacesSelectable)
				{
					vertexObject->ClearSelectedFaces();
				}
			}
		}

		EditModeSelectionType = InSelectionType;
	}

	bool Application::AreEditModeVerticesSelectable() const
	{
		return AreVerticesSelectableInGivenEditMode(EditModeSelectionType);
	}

	bool Application::AreEditModeEdgesSelectable() const
	{
		return AreEdgesSelectableInGivenEditMode(EditModeSelectionType);
	}

	bool Application::AreEditModeFacesSelectable() const
	{
		return AreFacesSelectableInGivenEditMode(EditModeSelectionType);
	}

	bool Application::AreVerticesSelectableInGivenEditMode(EEditModeSelectionType InSelectionType) const
	{
		return InSelectionType == EEditModeSelectionType::Vertex;;
	}

	bool Application::AreEdgesSelectableInGivenEditMode(EEditModeSelectionType InSelectionType) const
	{
		return InSelectionType == EEditModeSelectionType::Edge;;
	}

	bool Application::AreFacesSelectableInGivenEditMode(EEditModeSelectionType InSelectionType) const
	{
		return InSelectionType == EEditModeSelectionType::Face;;
	}

	void Application::TryToggleEditMode()
	{
		if (CurrentSceneMode == ESceneMode::Edit)
		{
			SetSceneMode(ESceneMode::Object);
		}
		else if (CurrentSceneMode == ESceneMode::Object && SelectedNodes.size() == 1 && SelectedNodes[0]->GetComponentPtr<RenderingComponent>() != nullptr)
		{
			SetSceneMode(ESceneMode::Edit);
		}
	}

	void Application::OnClick(int InClickIndex, glm::vec2 InMousePosition, glm::vec2 SceneViewSize)
	{
		if (CurrentSceneMode == ESceneMode::Edit)
		{
			FRay mouseRay = Scene_->GetActiveCameraPtr()->GenerateRay(InMousePosition, SceneViewSize);
			mouseRay.ApplyTransform(glm::inverse(SelectedNodes[0]->GetTransform().GetLocalToWorldMatrix()));
			SelectedNodes[0]->GetComponentPtr<RenderingComponent>()->GetVertexObjectPtr()->HandleClick(mouseRay, this);
		}
		else if (CurrentSceneMode == ESceneMode::Object)
		{
			// Try selecting with mouse ray
			auto& root = Scene_->GetRootNode();
			std::vector<RenderingComponent*> renderingComps = root.GetComponentPtrsInChildren<RenderingComponent>();

			FRay mouseRay = Scene_->GetActiveCameraPtr()->GenerateRay(InMousePosition, SceneViewSize);

			//std::cout << "Ray Origin: " << glm::to_string(mouseRay.GetOrigin()) << " Dir: " << glm::to_string(mouseRay.GetDirection()) << std::endl;
			//Scene_->AddDebugRay(mouseRay);

			FHitRecord record;
			bool objectHit = false;
			int indexHit = 0;

			for (size_t i = 0; i < renderingComps.size(); i++)
			{
				RenderingComponent* renderingComp = renderingComps[i];
				if (!renderingComp->bIsDebugRender)
				{
					VertexObject* vertexObject = renderingComp->GetVertexObjectPtr();
					std::shared_ptr<MeshHittable> hittable = std::make_shared<MeshHittable>(vertexObject->GetPositions(), vertexObject->GetNormals(), vertexObject->GetIndices(), vertexObject->GetTexCoords(), false);
					hittable->ModelMatrix = renderingComp->GetNodePtr()->GetTransform().GetLocalToWorldMatrix();
					hittable->InverseModelMatrix = glm::inverse(hittable->ModelMatrix);
					hittable->TransposeInverseModelMatrix = glm::transpose(hittable->InverseModelMatrix);

					// Cast a ray in object space for this hittable
					FRay objectSpaceRay = FRay(mouseRay.GetOrigin(), mouseRay.GetDirection());
					objectSpaceRay.ApplyTransform(hittable->InverseModelMatrix);
					Material defaultMat;
					bool bWasHitRecorded = hittable->Intersect(objectSpaceRay, .00001f, record, defaultMat);

					if (bWasHitRecorded)
					{
						objectHit = true;
						indexHit = (int)i;
					}
				}
			}

			if (objectHit)
			{
				SelectNode(renderingComps[indexHit]->GetNodePtr(), false);
			}
			else
			{
				DeselectAllNodes();
			}
		}
	}

	void Application::SelectAllEditModePrims()
	{
		if (CurrentSceneMode == ESceneMode::Edit)
		{
			VertexObject* obj = SelectedNodes[0]->GetComponentPtr<RenderingComponent>()->GetVertexObjectPtr();

			if (EditModeSelectionType == EEditModeSelectionType::Vertex)
			{
				obj->SelectAllVertices();
			}
			else if (EditModeSelectionType == EEditModeSelectionType::Edge)
			{
				obj->SelectAllEdges();
			}
			else if (EditModeSelectionType == EEditModeSelectionType::Face)
			{
				obj->SelectAllFaces();
			}
		}
	}

	void Application::HandleEditModeHotkeys()
	{
		if (ImGui::IsKeyPressed(GLFW_KEY_1))
		{
			SetEditModeSelectionType(EEditModeSelectionType::Vertex);
		}
		if (ImGui::IsKeyPressed(GLFW_KEY_2))
		{
			SetEditModeSelectionType(EEditModeSelectionType::Edge);
		}
		if (ImGui::IsKeyPressed(GLFW_KEY_3))
		{
			SetEditModeSelectionType(EEditModeSelectionType::Face);
		}
		if (ImGui::IsKeyPressed(GLFW_KEY_A))
		{
			SelectAllEditModePrims();
		}
		if (ImGui::IsKeyPressed(GLFW_KEY_L))
		{
			if (CurrentSceneMode == ESceneMode::Edit)
			{
				VertexObject* obj = SelectedNodes[0]->GetComponentPtr<RenderingComponent>()->GetVertexObjectPtr();
				if (obj->GetSelectedEdges().size() > 0)
				{
					obj->SelectEdgeLoop(obj->GetSelectedEdgesPtrs()[0]);
				}
			}
		}
		if (ImGui::IsKeyPressed(GLFW_KEY_N))
		{
			if (CurrentSceneMode == ESceneMode::Edit)
			{
				VertexObject* obj = SelectedNodes[0]->GetComponentPtr<RenderingComponent>()->GetVertexObjectPtr();
				if (obj->GetSelectedEdges().size() > 0)
				{
					if (ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
					{
						obj->SelectEdge(obj->GetSelectedEdgesPtrs()[0]->GetSecondHalfEdge()->GetNextHalfEdge()->GetOwningEdge()->GetIndexId(), false);
					}
					else
					{
						obj->SelectEdge(obj->GetSelectedEdgesPtrs()[0]->GetFirstHalfEdge()->GetNextHalfEdge()->GetOwningEdge()->GetIndexId(), false);
					}
				}
			}
		}
	}

	SceneNode* Application::CreatePrimitiveNode(EDefaultObject InObjectType, FDefaultObjectParams InParams)
	{
		std::shared_ptr<PhongShader> cubeShader = std::make_shared<PhongShader>();
		std::shared_ptr<VertexObject> cubeMesh = std::make_shared<VertexObject>(InObjectType, InParams);

		std::string name = "Object";
		switch (InObjectType)
		{
		case (EDefaultObject::Cube):
			name = "Cube";
			break;
		case (EDefaultObject::Plane):
			name = "Plane";
			break;
		case (EDefaultObject::Cylinder):
			name = "Cylinder";
			break;
		default:
			break;
		}
		auto cubeNode = make_unique<SceneNode>(fmt::format("{}.{}", name, Scene_->GetRootNode().GetChildrenCount()));
		cubeNode->CreateComponent<ShadingComponent>(cubeShader);
		cubeNode->CreateComponent<RenderingComponent>(cubeMesh);
		cubeNode->GetTransform().SetPosition(glm::vec3(0.f, 0.f, 0.f));
		auto material = MaterialManager::GetInstance().GetDefaultMaterial();
		auto hittableLight = std::make_shared<HittableLight>();
		cubeNode->CreateComponent<LightComponent>(hittableLight);
		cubeNode->CreateComponent<MaterialComponent>(material);
		SceneNode* ref = cubeNode.get();
		cubeNode->SetNodeType("Mesh");

		Scene_->GetRootNode().AddChild(std::move(cubeNode));
		return ref;
	}

	SceneNode* Application::CreateVertexObjectCopy(VertexObject* InVertexObjectToCopy)
	{
		std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();
		FDefaultObjectParams dummyParams;
		std::shared_ptr<VertexObject> mesh = std::make_shared<VertexObject>(EDefaultObject::CustomMesh, dummyParams);
		mesh->CopyVertexObject(InVertexObjectToCopy);
		mesh->MarkDirty();

		std::string name = "Copy";

		auto node = make_unique<SceneNode>(fmt::format("{}.{}", name, Scene_->GetRootNode().GetChildrenCount()));
		node->CreateComponent<ShadingComponent>(shader);
		node->CreateComponent<RenderingComponent>(mesh);
		node->GetTransform().SetPosition(glm::vec3(0.f, 0.f, 0.f));
		auto material = MaterialManager::GetInstance().GetDefaultMaterial();
		auto hittableLight = std::make_shared<HittableLight>();
		node->CreateComponent<LightComponent>(hittableLight);
		node->CreateComponent<MaterialComponent>(material);
		SceneNode* ref = node.get();
		node->SetNodeType("Mesh");
		Scene_->GetRootNode().AddChild(std::move(node));
		return ref;
	}

	SceneNode* Application::CreateCamera()
	{
		auto cameraNode = make_unique<TracingCameraNode>(fmt::format("Camera.{}", Scene_->GetRootNode().GetChildrenCount()));
		SceneNode* ref = cameraNode.get();
		cameraNode->SetNodeType("Camera");

		Scene_->GetRootNode().AddChild(std::move(cameraNode));
		return ref;
	}

	SceneNode* Application::CreatePointLight()
	{
		auto lightNode = make_unique<LightNode>(fmt::format("PointLight.{}", Scene_->GetRootNode().GetChildrenCount()));
		SceneNode* ref = lightNode.get();
		lightNode->SetNodeType("Light");

		Scene_->GetRootNode().AddChild(std::move(lightNode));
		return ref;
	}

	SceneNode* Application::CreateDirectionalLight()
	{
		std::unique_ptr<DirectionalLightNode> lightNode = make_unique<DirectionalLightNode>(fmt::format("DirectionalLight.{}", Scene_->GetRootNode().GetChildrenCount()));
		SceneNode* ref = lightNode.get();
		lightNode->SetNodeType("Light");

		Scene_->GetRootNode().AddChild(std::move(lightNode));
		return ref;
	}

	SceneNode* Application::CreateAmbientLight()
	{
		auto lightNode = make_unique<AmbientLightNode>(fmt::format("AmbientLight.{}", Scene_->GetRootNode().GetChildrenCount()));
		SceneNode* ref = lightNode.get();
		lightNode->SetNodeType("Light");

		Scene_->GetRootNode().AddChild(std::move(lightNode));
		return ref;
	}

	SceneNode* Application::CreateTracingSphereNode()
	{
		auto tracingNode = make_unique<TracingNode>(fmt::format("TracingSphere.{}", Scene_->GetRootNode().GetChildrenCount()));
		SceneNode* ref = tracingNode.get();

		tracingNode->SetNodeType("Tracing");
		if (MaterialComponent* mat = tracingNode->GetComponentPtr<MaterialComponent>())
		{
			mat->SetMaterial(MaterialManager::GetInstance().GetDefaultMaterial());
		}

		Scene_->GetRootNode().AddChild(std::move(tracingNode));
		return ref;
	}

	void Application::CreateImportMeshNode(const std::string& filePath, bool useImportedNormals)
	{
		std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();
		std::vector<std::shared_ptr<VertexObject>> meshes = std::move(MeshLoader::ImportObj(filePath, useImportedNormals));

		for (size_t i = 0; i < meshes.size(); i++)
		{
			auto meshNode = make_unique<SceneNode>(meshes[i]->ObjectName == "" ? fmt::format("Mesh.{}", Scene_->GetRootNode().GetChildrenCount()) : meshes[i]->ObjectName);
			meshNode->CreateComponent<ShadingComponent>(shader);
			meshNode->CreateComponent<RenderingComponent>(meshes[i]);
			meshNode->GetTransform().SetPosition(glm::vec3(0.f, 0.f, 0.f));
			auto material = MaterialManager::GetInstance().GetDefaultMaterial();
			if (meshes[i]->ImportedMaterialName != "")
			{
				auto importMaterial = MaterialManager::GetInstance().GetMaterial(meshes[i]->ImportedMaterialName);
				if (importMaterial)
					material = std::move(importMaterial);
			}
			meshNode->CreateComponent<MaterialComponent>(std::move(material));

			auto hittableLight = std::make_shared<HittableLight>();
			meshNode->CreateComponent<LightComponent>(hittableLight);

			SceneNode* ref = meshNode.get();
			meshNode->SetNodeType("Mesh");
			Scene_->GetRootNode().AddChild(std::move(meshNode));
		}
	}

	SceneNode* Application::CreateEmptyNode()
	{
		std::shared_ptr<SimpleShader> shader = std::make_shared<SimpleShader>();
		FDefaultObjectParams params;
		std::shared_ptr<VertexObject> mesh = std::make_shared<VertexObject>(EDefaultObject::Debug, params);

		auto vertexPositions = make_unique<FPositionArray>();
		float range = 1.0f;

		vertexPositions->push_back(glm::vec3(-range, 0.0f, 0.0f));
		vertexPositions->push_back(glm::vec3(range, 0.0f, 0.0f));
		vertexPositions->push_back(glm::vec3(0.0f, -range, 0.0f));
		vertexPositions->push_back(glm::vec3(0.0f, range, 0.0f));
		vertexPositions->push_back(glm::vec3(0.0f, 0.0f, -range));
		vertexPositions->push_back(glm::vec3(0.0f, 0.0f, range));
		mesh->UpdatePositions(std::move(vertexPositions));

		auto emptyNode = make_unique<SceneNode>(fmt::format("Empty.{}", Scene_->GetRootNode().GetChildrenCount()));
		auto& xAxisRendering = emptyNode->CreateComponent<RenderingComponent>(mesh);
		xAxisRendering.SetDrawMode(EDrawMode::Lines);
		xAxisRendering.bRenderSolid = false;
		xAxisRendering.bIsDebugRender = true;
		emptyNode->GetTransform().SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		auto blackMaterial = std::make_shared<Material>();
		blackMaterial->SetAlbedo(glm::vec3(0.0f));
		emptyNode->CreateComponent<ShadingComponent>(shader);
		auto matComp = emptyNode->CreateComponent<MaterialComponent>(blackMaterial, true);

		SceneNode* ref = emptyNode.get();
		emptyNode->SetNodeType("Empty");
		Scene_->GetRootNode().AddChild(std::move(emptyNode));
		return ref;
	}

	void Application::ResetScene()
	{
		DeselectAllNodes();
		SetSceneMode(ESceneMode::Object);
		Scene_ = make_unique<Scene>(make_unique<SceneNode>("Root"));
	}

	void Application::UpdateWindowFilename(std::string InFilename)
	{
		CurrentFilename = InFilename;
		std::string fileDisplay = CurrentFilename.empty() ? "Untitled" : CurrentFilename;
		glfwSetWindowTitle(WindowHandle, fmt::format("{} - {}", AppName, fileDisplay).c_str());
	}

	void Application::RecursiveUpdateToTimelineFrame(int InFrame, SceneNode* InSceneNode)
	{
		std::vector<IKeyframeable*> keyframeables = InSceneNode->GetKeyframeables();
		for (auto kf : keyframeables)
		{
			kf->ApplyKeyframeData(InFrame);
		}

		size_t childCount = InSceneNode->GetChildrenCount();
		for (size_t i = 0; i < childCount; i++)
		{
			RecursiveUpdateToTimelineFrame(InFrame, &InSceneNode->GetChild(i));
		}
	}

	void Application::UpdateToTimelineFrame(int InFrame)
	{
		//std::cout << "Updating to: " << InFrame << std::endl;
		RecursiveUpdateToTimelineFrame(InFrame, Scene_->GetRootNodePtr());
	}

	void Application::SetIsPreviewingRenderCamera(bool InIsPreviewing)
	{
		bIsPreviewingRenderCamera = InIsPreviewing;
		static_cast<ArcBallCameraNode*>(Scene_->GetActiveCameraPtr()->GetNodePtr())->bCanUpdate = !bIsPreviewingRenderCamera;
		if (!bIsPreviewingRenderCamera)
		{
			// Reset defaults
			CameraComponent* mainCam = Scene_->GetActiveCameraPtr()->GetNodePtr()->GetComponentPtr<CameraComponent>();
			mainCam->SetFOV(45.0f);
		}
	}

	void Application::InitializeGLFW()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		WindowHandle = glfwCreateWindow(WindowSize.x, WindowSize.y,
			AppName.c_str(), nullptr, nullptr);
		UpdateWindowFilename(CurrentFilename);

		if (WindowHandle == nullptr) {
			std::cerr << "Failed to create GLFW window!" << std::endl;
			return;
		}
		glfwMakeContextCurrent(WindowHandle);

		InputManager::GetInstance().SetWindow(WindowHandle);

		glfwSetWindowUserPointer(WindowHandle, this);
		glfwSetFramebufferSizeCallback(
			WindowHandle, +[](GLFWwindow* window, int width, int height) {
				static_cast<Application*>(glfwGetWindowUserPointer(window))
					->FramebufferSizeCallback(glm::ivec2(width, height));
			});

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cerr << "Failed to initialize GLAD!" << std::endl;
			return;
		}

		// On retina display, the initial window size will be larger
		// than requested.
		int initialWidth, initialHeight;
		glfwGetFramebufferSize(WindowHandle, &initialWidth, &initialHeight);
		FramebufferSizeCallback(glm::ivec2(initialWidth, initialHeight));
	}

	void Application::InitializeGUI()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;			// Enable Multi-Viewport / Platform Windows

		io.Fonts->AddFontFromFileTTF((GetAssetDir() + "fonts/FiraSans/FiraSans-Bold.ttf").c_str(), 14.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF((GetAssetDir() + "fonts/FiraSans/FiraSans-Regular.ttf").c_str(), 14.0f);

		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.08f, 0.63f, 0.95f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };


		// Setup Dear ImGui style
		//ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(WindowHandle, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void Application::UpdateGUI(float InDeltaTime)
	{
		// ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		DrawGUI(InDeltaTime);
	}

	void Application::RenderGUI()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void Application::DestroyGUI()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

}