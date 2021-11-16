#include "Scene.h"

namespace CHISTUDIO {

void Scene::Update(double InDeltaTime)
{
	RecursiveUpdate(*RootNode, InDeltaTime);
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