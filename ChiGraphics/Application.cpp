#include "Application.h"
#include <iostream>

#include "ChiGraphics/Utilities.h"
#include "ChiGraphics/InputManager.h"
#include "ChiGraphics/Collision/FRay.h"

namespace CHISTUDIO {

Application::Application(std::string InAppName, glm::ivec2 InWindowSize)
	: AppName(InAppName), WindowSize(InWindowSize)
{
	InitializeGLFW();
	InitializeGUI();

	CurrentSceneMode = ESceneMode::Object;
	EditModeSelectionType = EEditModeSelectionType::Vertex;
	Scene_ = make_unique<Scene>(make_unique<SceneNode>("Root"));
	Renderer_ = make_unique<Renderer>(*this);
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
	UpdateGUI();

	// Logic update before rendering.
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
		for (int i = 0; i < SelectedNodes.size(); i++)
		{
			SelectedNodes[i]->SetSelected(false);
		}
		SelectedNodes.clear();

		SetSceneMode(ESceneMode::Object);
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

void Application::SetSceneMode(ESceneMode InSceneMode)
{
	CurrentSceneMode = InSceneMode;
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

void Application::UpdateGUI()
{
	// ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	DrawGUI();
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