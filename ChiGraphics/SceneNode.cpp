#include "SceneNode.h"
#include <glm/gtx/string_cast.hpp>

namespace CHISTUDIO {

SceneNode::SceneNode(std::string InNodeName) : Transform_(*this), Parent(nullptr), Active(true), NodeName(InNodeName), bIsSelected(false)
{
}

void SceneNode::AddChild(std::unique_ptr<SceneNode> InChild)
{
	InChild->Parent = this;
	Children.emplace_back(std::move(InChild));
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

}