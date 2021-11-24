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
        FRayTraceSettings settings;
        //settings.BackgroundColor = glm::vec3(.5f, .7f, 1.0f);
        settings.BackgroundColor = glm::vec3(0.0f);
        settings.bShadowsEnabled = false;
        settings.ImageSize = glm::ivec2(RenderWidth, RenderHeight);
        settings.MaxBounces = MaxBounces;
        settings.SamplesPerPixel = SamplesPerPixel;
        settings.HDRI = HDRI.get();
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

    if (ImGui::Button("Load HDRI"))
    {
        std::string hdriFileName = UILibrary::PickFileName("Supported Files (*.hdr)\0*.hdr\0");
        if (hdriFileName.size() > 0)
        {
            HDRITexture = make_unique<FTexture>();
            HDRI = FImage::LoadPNG(hdriFileName, false);

            HDRITexture->Reserve(GL_RGB, HDRI->GetWidth(), HDRI->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE);
            HDRITexture->UpdateImage(*HDRI);
        }
    }

    if (HDRITexture)
    {
        uint64_t textureID = HDRITexture->GetHandle();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ (float)HDRITexture->Width * .2f, (float)HDRITexture->Height * .2f}, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    }

    ImGui::End();
}

}