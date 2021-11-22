#include "WRendering.h"
#include "ChiGraphics/Application.h"
#include "ChiGraphics/Scene.h"
#include "ChiGraphics/GL_Wrapper/FTexture.h"
#include "ChiGraphics/RayTracing/RayTracer.h"
#include "UILibrary.h"

namespace CHISTUDIO {

WRendering::WRendering()
{
    RenderWidth = 300;
    RenderHeight = 300;
    SamplesPerPixel = 1;
    MaxBounces = 1;
    FileName = "Output";
}

void WRendering::Render(Application& InApplication)
{
    ImGui::Begin("Rendering");

    Scene& scene = InApplication.GetScene();
    SceneNode& root = scene.GetRootNode();

    if (ImGui::Button("Render Scene"))
    {
        // Test reflect
        //glm::dvec3 wo = glm::dvec3(-0.001283480336415674, -0.007632098157135869, 0.9999700514295146);
        //glm::dvec3 h = glm::dvec3(0.011573625529115436, 0.06880514329354218, 0.9975629821963458);
        //std::cout << "Reflect: " << glm::to_string(-glm::reflect(wo, h)) << std::endl;

        FRayTraceSettings settings;
        settings.BackgroundColor = glm::vec3(.2f);
        settings.bShadowsEnabled = false;
        settings.ImageSize = glm::ivec2(RenderWidth, RenderHeight);
        settings.MaxBounces = MaxBounces;
        settings.SamplesPerPixel = SamplesPerPixel;
        FRayTracer rayTracer(settings);

        DisplayTexture = rayTracer.Render(scene, fmt::format("{}.png", FileName));
    }

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    std::strncpy(buffer, FileName.c_str(), sizeof(buffer));
    if (ImGui::InputText("File Name", buffer, sizeof(buffer)))
    {
        FileName = std::string(buffer);
    }

    ImGui::PushMultiItemsWidths(4, 150);

    ImGui::DragInt("Size X", &RenderWidth);
    ImGui::PopItemWidth();
    ImGui::DragInt("Size Y", &RenderHeight);
    ImGui::PopItemWidth();
    ImGui::DragInt("Max Bounces", &MaxBounces, 1.0f, 0, 5);
    ImGui::PopItemWidth();
    ImGui::DragInt("Samples Per Pixel", &SamplesPerPixel, 1.0f, 1, 300);
    ImGui::PopItemWidth();

    if (DisplayTexture)
    {
        uint64_t textureID = DisplayTexture->GetHandle();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ (float)DisplayTexture->Width, (float)DisplayTexture->Height }, ImVec2{ 0, 0 }, ImVec2{ 1, 1 });
    }

    ImGui::End();
}

}