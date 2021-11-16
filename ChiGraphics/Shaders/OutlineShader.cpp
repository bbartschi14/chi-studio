#include "OutlineShader.h"
#include "ChiGraphics/Components/RenderingComponent.h"
#include "ChiGraphics/Components/MaterialComponent.h"
#include "ChiGraphics/Components/CameraComponent.h"
#include "ChiGraphics/Components/LightComponent.h"
#include "ChiGraphics/Lights/AmbientLight.h"
#include "ChiGraphics/Lights/PointLight.h"
#include "ChiGraphics/SceneNode.h"
#include <stdexcept>
#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>


namespace CHISTUDIO {

OutlineShader::OutlineShader()
    : ShaderProgram(std::unordered_map<GLenum, std::string>
    {
        {GL_VERTEX_SHADER, "outline.vert"},
        { GL_FRAGMENT_SHADER, "outline.frag" }
    })
{
}

void OutlineShader::SetTargetNode(const SceneNode& InNode, const glm::mat4& InModelMatrix) const
{
    // Associate the right VAO before rendering.
    AssociateVertexArray(InNode.GetComponentPtr<RenderingComponent>()->GetVertexObjectPtr()->GetVertexArray());

    // Set transform.
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(InModelMatrix)));
    SetUniform("model_matrix", InModelMatrix);
    SetUniform("normal_matrix", normalMatrix);
}

void OutlineShader::SetCamera(const CameraComponent& InCamera) const
{
    SetUniform("view_matrix", InCamera.GetViewMatrix());
    SetUniform("projection_matrix", InCamera.GetProjectionMatrix());
    SetUniform("camera_position", InCamera.GetAbsoluteCameraPosition());
}

/*void OutlineShader::SetTexture(const FTexture& texture, const FTexture& depthStencilTexture) const
{
    texture.BindToUnit(0);
    SetUniform("in_texture", 0);

    depthStencilTexture.BindToUnit(1);
    SetUniform("in_stencil_mask_texture", 1);
}*/

void OutlineShader::AssociateVertexArray(const VertexArray& InVertexArray) const
{
    if (!InVertexArray.HasPositionBuffer()) {
        throw std::runtime_error("Phong shader requires vertex positions!");
    }
    if (!InVertexArray.HasNormalBuffer()) {
        throw std::runtime_error("Phong shader requires vertex normals!");
    }

    InVertexArray.LinkPositionBuffer(GetAttributeLocation("vertex_position"));
    InVertexArray.LinkNormalBuffer(GetAttributeLocation("vertex_normal"));
}


}