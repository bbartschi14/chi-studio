#pragma once

#include "ShaderProgram.h"
#include "ChiGraphics/GL_Wrapper/FTexture.h"
#include "ChiGraphics/Meshes/VertexObject.h"

namespace CHISTUDIO {

class OutlineShader : public ShaderProgram
{
public:
    OutlineShader();
    void SetTargetNode(const SceneNode& InNode, const glm::mat4& InModelMatrix) const;
    void SetCamera(const CameraComponent& InCamera) const;
    //void SetTexture(const FTexture& texture, const FTexture& depthStencilTexture) const;

private:
    void AssociateVertexArray(const VertexArray& InVertexArray) const;
};

}