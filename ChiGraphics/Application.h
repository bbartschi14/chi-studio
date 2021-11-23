#ifndef CHISTUDIO_APPLICATION_H_
#define CHISTUDIO_APPLICATION_H_

#include <memory>
#include <string>
#include "External.h"
#include "Scene.h"
#include "Renderer.h"

namespace CHISTUDIO {

enum class ESceneMode { Object, Edit };
enum class EEditModeSelectionType { Vertex, Edge, Face };

class Application
{
public: 
	Application(std::string InAppName, glm::ivec2 InWindowSize);
	virtual ~Application();

	bool IsFinished();
	void Tick(double InDeltaTime, double InCurrentTime);

	glm::ivec2 GetWindowSize() const
	{
		return WindowSize;
	}

	Scene& GetScene() const
	{
		return *Scene_;
	}

	virtual void FramebufferSizeCallback(glm::ivec2 InWindowSize);

	std::vector<SceneNode*> GetSelectedNodes() const
	{
		return SelectedNodes;
	}

	/** Can either add to the selection, or replace the selection */
	void SelectNode(SceneNode* nodeToSelect, bool addToSelection);

	ESceneMode GetSceneMode() const {
		return CurrentSceneMode;
	}

	void SetSceneMode(ESceneMode InSceneMode);

	EEditModeSelectionType GetEditModeSelectionType() const {
		return EditModeSelectionType;
	}

	void SetEditModeSelectionType(EEditModeSelectionType InSelectionType);

	bool AreEditModeVerticesSelectable() const;
	bool AreEditModeEdgesSelectable() const;
	bool AreEditModeFacesSelectable() const;

	bool AreVerticesSelectableInGivenEditMode(EEditModeSelectionType InSelectionType) const;
	bool AreEdgesSelectableInGivenEditMode(EEditModeSelectionType InSelectionType) const;
	bool AreFacesSelectableInGivenEditMode(EEditModeSelectionType InSelectionType) const;

	void TryToggleEditMode();

	void OnClick(int InClickIndex, glm::vec2 InMousePosition, glm::vec2 SceneViewSize);

	SceneNode* CreatePrimitiveNode(EDefaultObject InObjectType);
	SceneNode* CreateCamera();
	SceneNode* CreatePointLight();
	SceneNode* CreateAmbientLight();
	SceneNode* CreateTracingSphereNode();
	SceneNode* CreateImportMeshNode(const std::string& filePath);

protected:
	virtual void DrawGUI() {}
	virtual void SetupScene() = 0;
	std::unique_ptr<Scene> Scene_;
	std::unique_ptr<Renderer> Renderer_;
	std::vector<SceneNode*> SelectedNodes;

	ESceneMode CurrentSceneMode;
	EEditModeSelectionType EditModeSelectionType;
private:
	void InitializeGLFW();

	// GUI Handlers
	void InitializeGUI();
	void UpdateGUI();
	void RenderGUI();
	void DestroyGUI();

	GLFWwindow* WindowHandle;
	std::string AppName;
	glm::ivec2 WindowSize;

};

}


#endif // !CHISTUDIO_APPLICATION_H_