#pragma once

#include <vector>
#include <memory>
#include "SceneNode.h"
#include "Components/CameraComponent.h"
#include "ChiGraphics/Collision/FRay.h"
#include "ChiGraphics/Components/CameraComponent.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/InputManager.h"
#include "ChiGraphics/Components/ShadingComponent.h"
#include "ChiGraphics/Shaders/SimpleShader.h"
#include "ChiGraphics/Components/MaterialComponent.h"

namespace CHISTUDIO {

class Scene
{
public:
    Scene(std::unique_ptr<SceneNode> InRootNode)
        : RootNode(std::move(InRootNode)), ActiveCameraPtr(nullptr) {
        std::shared_ptr<SimpleShader> shader = std::make_shared<SimpleShader>();
        FDefaultObjectParams params;
        std::shared_ptr<VertexObject> mesh = std::make_shared<VertexObject>(EDefaultObject::Debug, params);
        RayPositions = make_unique<FPositionArray>();
        auto copyRayPositions = make_unique<FPositionArray>();
        copyRayPositions->push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        copyRayPositions->push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        mesh->UpdatePositions(std::move(copyRayPositions));
        auto& rendering = RootNode->CreateComponent<RenderingComponent>(mesh);
        rendering.SetDrawMode(EDrawMode::Lines);
        rendering.bRenderSolid = false;
        rendering.bIsDebugRender = true;
        auto xMaterial = std::make_shared<Material>();
        xMaterial->SetAlbedo(glm::vec3(.6f, 0.15f, 0.0f));
        RootNode->CreateComponent<ShadingComponent>(shader);
        RootNode->CreateComponent<MaterialComponent>(xMaterial);
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
    void AddDebugRay(FRay InRay);

private:
    void RecursiveUpdate(SceneNode& InNode, double InDeltaTime);

    std::unique_ptr<SceneNode> RootNode;
    CameraComponent* ActiveCameraPtr;
    std::unique_ptr<FPositionArray> RayPositions;

};

}

