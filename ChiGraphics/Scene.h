#pragma once

#include <vector>
#include <memory>
#include "SceneNode.h"
#include "Components/CameraComponent.h"

namespace CHISTUDIO {

class Scene
{
public:
    Scene(std::unique_ptr<SceneNode> InRootNode)
        : RootNode(std::move(InRootNode)), ActiveCameraPtr(nullptr) {
    }
    SceneNode& GetRootNode() {
        return *RootNode;
    }
    const SceneNode& GetRootNode() const {
        return *RootNode;
    }
    void ActivateCamera(CameraComponent* InCameraPtr) {
        ActiveCameraPtr = InCameraPtr;
    }
    CameraComponent* GetActiveCameraPtr() const {
        return ActiveCameraPtr;
    }

    void Update(double InDeltaTime);

private:
    void RecursiveUpdate(SceneNode& InNode, double InDeltaTime);

    std::unique_ptr<SceneNode> RootNode;
    CameraComponent* ActiveCameraPtr;
};

}

