#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <typeinfo>
#include <stdexcept>

#include <glm/vec3.hpp>

#include "Components/ComponentBase.h"
#include "Components/ComponentType.h"
#include "Transform.h"

namespace CHISTUDIO {

class SceneNode
{
public:
    SceneNode(std::string InNodeName);
    virtual ~SceneNode() {
    }

    size_t GetChildrenCount() const {
        return Children.size();
    }

    SceneNode& GetChild(size_t InIndex) const {
        return *Children.at(InIndex);
    }

    SceneNode* GetChildPtr(size_t InIndex) const {
        return Children.at(InIndex).get();
    }

    SceneNode* GetParentPtr() const {
        return Parent;
    }

    void AddChild(std::unique_ptr<SceneNode> InChild);
    std::unique_ptr<SceneNode> RemoveChild(SceneNode* InChildToRemove);

    template <class T>
    void AddComponent(std::unique_ptr<T> InComponent) {
        InComponent->SetNodePtr(this);
        ComponentDictionary[ComponentTrait<T>::GetType()] = std::move(InComponent);
    }

    template <class T>
    bool RemoveComponent() {
        auto itr = ComponentDictionary.find(ComponentTrait<T>::GetType());
        if (itr != ComponentDictionary.end()) {
            ComponentDictionary.erase(itr);
            return true;
        }
        return false;
    }

    template <class T, typename... Args>
    T& CreateComponent(Args&&... args) {
        AddComponent(make_unique<T>(std::forward<Args>(args)...));
        return *GetComponentPtr<T>();
    }

    template <class T>
    T* GetComponentPtr() const {
        // Returns pointer since the component may not exist.
        return static_cast<T*>(GetComponentPtrByType(ComponentTrait<T>::GetType()));
    }

    template <class T>
    std::vector<T*> GetComponentPtrsInChildren() const {
        std::vector<T*> result;
        std::vector<ComponentBase*> result_raw =
            GetComponentsPtrInChildrenByType(ComponentTrait<T>::GetType());
        for (ComponentBase* c : result_raw) {
            result.push_back(static_cast<T*>(c));
        }
        return result;
    }

    Transform& GetTransform() {
        return Transform_;
    }

    // Const overloading.
    const Transform& GetTransform() const {
        return Transform_;
    }

    bool IsActive() const {
        return Active;
    }
    void SetActive(bool InNewState) {
        Active = InNewState;
    }

    virtual void Update(double InDeltaTime) {
    }

    const std::string& GetNodeName() const {
        return NodeName;
    }

    void SetNodeName(const std::string& InName)
    {
        NodeName = InName;
    }

    void SetSelected(bool InIsSelected) {
        bIsSelected = InIsSelected;
    }

    bool IsSelected() const {
        return bIsSelected;
    }

    void SetHierarchyVisible(bool InIsHierarchyVisible) {
        bIsHierarchyVisible = InIsHierarchyVisible;
    }

    bool IsHierarchyVisible() const {
        return bIsHierarchyVisible;
    }

private:
    ComponentBase* GetComponentPtrByType(EComponentType InType) const;

    std::vector<ComponentBase*> GetComponentsPtrInChildrenByType(EComponentType InType) const;

    void GatherComponentPtrsRecursivelyByType(EComponentType InType, std::vector<ComponentBase*>& result) const;

    Transform Transform_;
    std::unordered_map<EComponentType, std::unique_ptr<ComponentBase>, EnumClassHash> ComponentDictionary;
    std::vector<std::unique_ptr<SceneNode>> Children;
    SceneNode* Parent;
    bool Active;
    std::string NodeName;
    bool bIsSelected;
    bool bIsHierarchyVisible;

};

}

