#include "Scene.h"

namespace CHISTUDIO {

    SceneNode* Scene::GetTracingCameraNode() const
    {
		auto& root = GetRootNode();
		std::vector<CameraComponent*> cameraComps = root.GetComponentPtrsInChildren<CameraComponent>();

		for (CameraComponent* cameraComp : cameraComps)
		{
			SceneNode* node = cameraComp->GetNodePtr();
			if (node->GetNodeType() == "Camera")
			{
				return node;
			}
		}

		return nullptr;
    }

    void Scene::Update(double InDeltaTime)
{
	RecursiveUpdate(*RootNode, InDeltaTime);
}

void Scene::AddDebugRay(FRay InRay)
{
    auto rendering = RootNode->GetComponentPtr<RenderingComponent>();
    RayPositions->push_back(InRay.GetOrigin());
    RayPositions->push_back(InRay.GetOrigin() + InRay.GetDirection() * 10000.0f);

    auto copyRayPositions = make_unique<FPositionArray>();
    for (size_t i = 0; i < RayPositions->size(); i++)
    {
        copyRayPositions->push_back(RayPositions->at(i));
    }
    if (rendering)
    {
        rendering->GetVertexObjectPtr()->UpdatePositions(std::move(copyRayPositions));
    }
}

void Scene::RecursiveUpdate(SceneNode& InNode, double InDeltaTime)
{
	InNode.Update(InDeltaTime);
	size_t childCount = InNode.GetChildrenCount();
	for (size_t i = 0; i < childCount; i++)
	{
		RecursiveUpdate(InNode.GetChild(i), InDeltaTime);
	}
}

}