#pragma once
#include "ShaderProgram.h"

namespace CHISTUDIO {

class PhongShader : public ShaderProgram 
{
public:
    PhongShader();
    void SetTargetNode(const SceneNode& InNode, const glm::mat4& InModelMatrix) const override;
    void SetCamera(const CameraComponent& InCamera) const override;
    void SetLightSource(const LightComponent& InComponent) const override;

private:
    void AssociateVertexArray(VertexArray& InVertexArray) const;
};

}
