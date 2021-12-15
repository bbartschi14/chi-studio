#include "SceneNode.h"
#include <glm/gtx/string_cast.hpp>
#include "Components/CameraComponent.h"
#include "Components/MaterialComponent.h"
namespace CHISTUDIO {

SceneNode::SceneNode(std::string InNodeName) : Transform_(*this), Parent(nullptr), Active(true), NodeName(InNodeName), bIsSelected(false), bIsHierarchyVisible(true)
{
}

void SceneNode::AddChild(std::unique_ptr<SceneNode> InChild)
{
	InChild->Parent = this;
	Children.emplace_back(std::move(InChild));
}

std::unique_ptr<SceneNode> SceneNode::RemoveChild(SceneNode* InChildToRemove)
{
	int indexToRemove = -1;
	for (size_t i = 0; i < Children.size(); i++)
	{
		if (Children[i].get() == InChildToRemove)
		{
			indexToRemove = (int)i;
			break;
		}
	}

	std::unique_ptr<SceneNode> child = std::move(Children[indexToRemove]);

	if (indexToRemove > -1)
	{
		Children.erase(Children.begin() + indexToRemove);
	}

	return child;
}

ComponentBase* SceneNode::GetComponentPtrByType(EComponentType InType) const
{
	if (IsActive() && ComponentDictionary.count(InType)) {
		return ComponentDictionary.at(InType).get();
	}

	return nullptr;
}

std::vector<ComponentBase*> SceneNode::GetComponentsPtrInChildrenByType(EComponentType InType) const
{
	std::vector<ComponentBase*> result;
	GatherComponentPtrsRecursivelyByType(InType, result);
	return result;
}

void SceneNode::GatherComponentPtrsRecursivelyByType(EComponentType InType, std::vector<ComponentBase*>& result) const
{
	ComponentBase* component = GetComponentPtrByType(InType);
	if (component != nullptr) {
		result.push_back(component);
	}
	size_t child_count = GetChildrenCount();
	for (size_t i = 0; i < child_count; i++) {
		SceneNode& child = GetChild(i);
		if (child.IsActive()) {
			child.GatherComponentPtrsRecursivelyByType(InType, result);
		}
	}
}

std::vector<IKeyframeable*> SceneNode::GetKeyframeables()
{
	std::vector<IKeyframeable*> keyframeables;
	keyframeables.push_back(&Transform_);

	if (CameraComponent* camera = GetComponentPtr<CameraComponent>())
	{
		keyframeables.push_back(camera);
	}
	if (MaterialComponent* materialComp = GetComponentPtr<MaterialComponent>())
	{
		keyframeables.push_back(materialComp->GetMaterialPtr());
	}

	return keyframeables;
}

}