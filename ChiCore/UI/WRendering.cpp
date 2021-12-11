#include "WRendering.h"
#include "ChiGraphics/Application.h"
#include "ChiGraphics/Scene.h"
#include "ChiGraphics/GL_Wrapper/FTexture.h"
#include "ChiGraphics/RayTracing/RayTracer.h"
#include "UILibrary.h"
#include <glm/gtc/type_ptr.hpp>

namespace CHISTUDIO {

WRendering::WRendering()
{
    RenderWidth = 300;
    RenderHeight = 300;
    SamplesPerPixel = 1;
    MaxBounces = 1;
    FileName = "Output";
    bUseHDRI = false;
    BackgroundColor = glm::vec3(.5f, .7f, 1.0f);
    ResultZoomScale = 1.0f;
}

void WRendering::Render(Application& InApplication)
{
    ImGui::Begin("Rendering");

    Scene& scene = InApplication.GetScene();
    SceneNode& root = scene.GetRootNode();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
    ImGui::BeginChild("RenderSettings", ImVec2{ 400, 200 }, true, window_flags);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Settings"))
        {
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    std::strncpy(buffer, FileName.c_str(), sizeof(buffer));
    ImGui::PushMultiItemsWidths(5, 1200);

    if (ImGui::InputText("File Name", buffer, sizeof(buffer)))
    {
        FileName = std::string(buffer);
    }

    ImGui::SliderInt("Size X", &RenderWidth, 0, 4096);
    ImGui::PopItemWidth();
    ImGui::SliderInt("Size Y", &RenderHeight, 0, 4096);
    ImGui::PopItemWidth();
    ImGui::SliderInt("Max Bounces", &MaxBounces, 0, 10);
    ImGui::PopItemWidth();
    ImGui::SliderInt("Samples Per Pixel", &SamplesPerPixel, 1, 1000);
    ImGui::PopItemWidth();
    ImGui::EndChild();

    ImGui::SameLine();

    float environmentPanelWidth = ImGui::GetContentRegionAvailWidth();//600.0f;
    ImGui::BeginChild("RenderEnvironment", ImVec2{ environmentPanelWidth, 200.0f }, true, window_flags);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Environment"))
        {
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::Checkbox("Use HDRI", &bUseHDRI);
    if (bUseHDRI)
    {
        ImGui::Text(fmt::format("HDRI: {}", HDRI ? HDRI->ImportedFileName : "None").c_str());
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
        ImGui::SameLine();
        if (ImGui::Button("Clear HDRI"))
        {
            HDRITexture = nullptr;
            HDRI = nullptr;
        }

        if (HDRITexture)
        {
            uint64_t textureID = HDRITexture->GetHandle();
            float scaleFactor = environmentPanelWidth / (float)HDRITexture->Width;
            ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ HDRITexture->Width * scaleFactor, HDRITexture->Height * scaleFactor }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        }
    }
    else
    {
        ImGui::ColorEdit3("Background Color", glm::value_ptr(BackgroundColor));
    }
    ImGui::EndChild();


    if (ImGui::Button("Render Scene", ImVec2{ 400,0 }))
    {
        FRayTraceSettings settings;
        settings.BackgroundColor = BackgroundColor;
        //settings.BackgroundColor = glm::vec3(0.0f);
        settings.bShadowsEnabled = false;
        settings.ImageSize = glm::ivec2(RenderWidth, RenderHeight);
        settings.MaxBounces = MaxBounces;
        settings.SamplesPerPixel = SamplesPerPixel;
        settings.HDRI = HDRI.get();
        settings.UseHDRI = bUseHDRI;
        FRayTracer rayTracer(settings);

        DisplayTexture = rayTracer.Render(scene, fmt::format("{}.png", FileName));
    }
    ImGui::SameLine();
    ImGui::SliderFloat("Image Zoom", &ResultZoomScale, 0.1f, 10.0f);

    ImGui::BeginChild("RenderResult", ImGui::GetContentRegionAvail(), true, window_flags | ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Result"))
        {
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (DisplayTexture)
    {
        uint64_t textureID = DisplayTexture->GetHandle();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ (float)DisplayTexture->Width * ResultZoomScale, (float)DisplayTexture->Height * ResultZoomScale }, ImVec2{ 0, 0 }, ImVec2{ 1, 1 });
    }
    ImGui::EndChild();
    

    ImGui::End();
}

}