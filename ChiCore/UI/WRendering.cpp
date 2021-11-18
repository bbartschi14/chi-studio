#include "WRendering.h"
#include "ChiGraphics/Application.h"
#include "ChiGraphics/Scene.h"
#include "ChiGraphics/GL_Wrapper/FTexture.h"
#include "ChiGraphics/RayTracing/RayTracer.h"
#include "UILibrary.h"

namespace CHISTUDIO {

WRendering::WRendering()
{
    RenderWidth = 800;
    RenderHeight = 500;
}

void WRendering::Render(Application& InApplication)
{
    ImGui::Begin("Rendering");

    Scene& scene = InApplication.GetScene();
    SceneNode& root = scene.GetRootNode();

    if (ImGui::Button("Render Scene"))
    {
        FRayTraceSettings settings;
        settings.BackgroundColor = glm::vec3(.1f);
        settings.bShadowsEnabled = false;
        settings.ImageSize = glm::ivec2(RenderWidth, RenderHeight);
        settings.MaxBounces = 4;

        FRayTracer rayTracer(settings);

        DisplayTexture = rayTracer.Render(scene, "test.png");
    }

    ImGui::PushMultiItemsWidths(2, 150);
    ImGui::DragInt("Render X", &RenderWidth);
    ImGui::PopItemWidth();
    ImGui::DragInt("Render Y", &RenderHeight);
    ImGui::PopItemWidth();

    if (DisplayTexture)
    {
        uint64_t textureID = DisplayTexture->GetHandle();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ (float)DisplayTexture->Width, (float)DisplayTexture->Height }, ImVec2{ 0, 0 }, ImVec2{ 1, 1 });
    }

    ImGui::End();
}

}