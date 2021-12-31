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
	
	// Finds all nodes in the visible hierarchy that don't have InSceneNode as an ancestor
	void GetNonChildNodes(SceneNode* InStartingNode, SceneNode* InSceneNode, std::vector<SceneNode*>& OutNodes) const
	{
		size_t numChildren = InStartingNode->GetChildrenCount();
		for (size_t i = 0; i < numChildren; i++)
		{
			SceneNode* child = InStartingNode->GetChildPtr(i);
			if (child->IsHierarchyVisible() && child != InSceneNode)
			{
				OutNodes.emplace_back(child);
				GetNonChildNodes(child, InSceneNode, OutNodes);
			}
		}

	}

	/** Can either add to the selection, or replace the selection */
	void SelectNode(SceneNode* nodeToSelect, bool addToSelection);
	void DeselectNode(SceneNode* nodeToDeselect);
	void DeselectAllNodes();

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
	void SelectAllEditModePrims();

	void HandleEditModeHotkeys();

	SceneNode* CreatePrimitiveNode(EDefaultObject InObjectType, FDefaultObjectParams InParams);
	SceneNode* CreateVertexObjectCopy(VertexObject* InVertexObjectToCopy);
	SceneNode* CreateCamera();
	SceneNode* CreatePointLight();
	SceneNode* CreateAmbientLight();
	SceneNode* CreateTracingSphereNode();
	SceneNode* CreateImportMeshNode(const std::string& filePath);
	SceneNode* CreateEmptyNode();

	// Clear our scene nodes and recreate scene
	void ResetScene();

	void UpdateWindowFilename(std::string InFilename);

	void RecursiveUpdateToTimelineFrame(int InFrame, SceneNode* InSceneNode);
	void UpdateToTimelineFrame(int InFrame);

	bool GetIsPreviewingRenderCamera() const { return bIsPreviewingRenderCamera; }
	void SetIsPreviewingRenderCamera(bool InIsPreviewing);
protected:
	virtual void DrawGUI(float InDeltaTime) {}
	virtual void SetupScene(bool InIncludeDefaults) = 0;
	std::unique_ptr<Scene> Scene_;
	std::unique_ptr<Renderer> Renderer_;
	std::vector<SceneNode*> SelectedNodes;

	ESceneMode CurrentSceneMode;
	EEditModeSelectionType EditModeSelectionType;
	std::string CurrentFilename;

	bool bIsPreviewingRenderCamera;
private:
	void InitializeGLFW();

	// GUI Handlers
	void InitializeGUI();
	void UpdateGUI(float InDeltaTime);
	void RenderGUI();
	void DestroyGUI();

	GLFWwindow* WindowHandle;
	std::string AppName;
	glm::ivec2 WindowSize;

};

}


#endif // !CHISTUDIO_APPLICATION_H_