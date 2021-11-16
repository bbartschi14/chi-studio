#pragma once

#include "ChiGraphics/Components/LightComponent.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/GL_Wrapper/FrameBuffer.h"
#include "ChiGraphics/GL_Wrapper/RenderBuffer.h"
#include <unordered_map>

namespace CHISTUDIO {

class Scene;
class Application;

class Renderer 
{
public:
    Renderer(Application& InApplication);
    void Render(const Scene& InScene);

    uint64_t GetSceneOutputTextureHandle() const;
private:
    using RenderingInfo = std::vector<std::pair<RenderingComponent*, glm::mat4>>;
    void RenderScene(const Scene& InScene) const;
    void SetRenderingOptions() const;

    RenderingInfo RetrieveRenderingInfo(const Scene& InScene) const;
    static void RecursiveRetrieve(const SceneNode& InNode, RenderingInfo& InInfo, const glm::mat4& InModelMatrix);
    Application& Application_;

    void InvalidateFrameBuffer();

    std::unique_ptr<Framebuffer> SceneColorFrameBuffer;
    std::unique_ptr<FTexture> SceneOutputTexture;
    std::unique_ptr<FTexture> SceneDepthStencilTexture;
    std::shared_ptr<class OutlineShader> OutlineShader_;
    std::shared_ptr<class PointShader> PointShader_;
    std::shared_ptr<class EdgeShader> EdgeShader_;
    std::shared_ptr<class SelectedFaceShader> SelectedFaceShader_;

    std::unique_ptr<VertexObject> SceneRenderQuad;


    uint32_t CurrentWidth;
    uint32_t CurrentHeight;

};

}
