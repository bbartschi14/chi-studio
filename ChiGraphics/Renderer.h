#pragma once

#include "ChiGraphics/Components/LightComponent.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/GL_Wrapper/FrameBuffer.h"
#include "ChiGraphics/GL_Wrapper/RenderBuffer.h"
#include <unordered_map>
#include "external/src/ImGuizmo/ImGuizmo.h"

namespace CHISTUDIO {

class Scene;
class Application;
enum class CustomImGuizmoMode
{
    HIDDEN,
    TRANSLATE,
    ROTATE,
    SCALE
};

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
    std::unique_ptr<Framebuffer> PostProcessFrameBuffer;
    std::unique_ptr<FTexture> PostProcessOutputTexture;
    std::unique_ptr<FTexture> SceneOutputTexture;
    std::unique_ptr<FTexture> SceneDepthStencilTexture;
    std::shared_ptr<class OutlineShader> OutlineShader_;
    std::shared_ptr<class PointShader> PointShader_;
    std::shared_ptr<class EdgeShader> EdgeShader_;
    std::shared_ptr<class SelectedFaceShader> SelectedFaceShader_;
    std::shared_ptr<class TextureBlendShader> TextureBlendShader_;
    std::shared_ptr<class TextureBlurShader> TextureBlurShader_;

    std::unique_ptr<FTexture> customMaskTexture;    
    std::unique_ptr<FTexture> OutlineTexture;

    std::unique_ptr<VertexObject> SceneRenderQuad;


    uint32_t CurrentWidth;
    uint32_t CurrentHeight;

    CustomImGuizmoMode GizmoOperationType;
    ImGuizmo::MODE GizmoSpace;

    // Gizmo helpers
    glm::vec3 StartingScaleOrigin;
    std::vector<glm::vec3> PreScaleVertexPositions;
    bool bIsScaling;
};

}
