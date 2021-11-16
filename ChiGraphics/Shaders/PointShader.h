#pragma once
#include "ShaderProgram.h"
#include "ChiGraphics/GL_Wrapper/FTexture.h"
#include "ChiGraphics/Meshes/VertexObject.h"

namespace CHISTUDIO {

    class PointShader : public ShaderProgram
    {
    public:
        PointShader();
        void SetTargetNode(const SceneNode& InNode, const glm::mat4& InModelMatrix) const;
        void SetCamera(const CameraComponent& InCamera) const;
        void AssociateVertexArray(const VertexArray& InVertexArray) const;
        void SetSelectedStyle(bool bIsSelected) const;
    };

}