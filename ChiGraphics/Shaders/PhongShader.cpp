#include "PhongShader.h"
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

PhongShader::PhongShader()
    : ShaderProgram(std::unordered_map<GLenum, std::string>
{
        {GL_VERTEX_SHADER, "phong.vert"},
        { GL_FRAGMENT_SHADER, "phong.frag" }
}) 
{
}

void PhongShader::SetTargetNode(const SceneNode& InNode, const glm::mat4& InModelMatrix) const
{
    // Associate the right VAO before rendering.
    AssociateVertexArray(InNode.GetComponentPtr<RenderingComponent>()->GetVertexObjectPtr()->GetVertexArray());

    // Set transform.
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(InModelMatrix)));
    SetUniform("model_matrix", InModelMatrix);
    SetUniform("normal_matrix", normalMatrix);

    // Set material.
    MaterialComponent* materialComponentPtr = InNode.GetComponentPtr<MaterialComponent>();

    const Material* materialPtr;
    if (materialComponentPtr == nullptr) {
        materialPtr = &Material::GetDefault();
    }
    else {
        materialPtr = &materialComponentPtr->GetMaterial();
    }
    SetUniform("material.ambient", materialPtr->GetAmbientColor());
    SetUniform("material.diffuse", materialPtr->GetDiffuseColor());
    SetUniform("material.specular", materialPtr->GetSpecularColor());
    SetUniform("material.shininess", materialPtr->GetShininess());
}

void PhongShader::SetCamera(const CameraComponent& InCamera) const
{
    SetUniform("view_matrix", InCamera.GetViewMatrix());
    SetUniform("projection_matrix", InCamera.GetProjectionMatrix());
    SetUniform("camera_position", InCamera.GetAbsoluteCameraPosition());
}
void PhongShader::SetLightSource(const LightComponent& InComponent) const
{
    auto light_ptr = InComponent.GetLightPtr();
    if (light_ptr == nullptr) {
        throw std::runtime_error("Light component has no light attached!");
    }

    // First disable all lights.
    // In a single rendering pass, only one light of one type is enabled.
    SetUniform("ambient_light.enabled", false);
    SetUniform("point_light.enabled", false);

    if (light_ptr->GetType() == ELightType::Ambient) {
        auto ambient_light_ptr = static_cast<AmbientLight*>(light_ptr);
        SetUniform("ambient_light.enabled", true);
        SetUniform("ambient_light.ambient", ambient_light_ptr->GetAmbientColor());
    }
    else if (light_ptr->GetType() == ELightType::Point) {
        auto point_light_ptr = static_cast<PointLight*>(light_ptr);
        SetUniform("point_light.enabled", true);
        SetUniform("point_light.position", InComponent.GetNodePtr()->GetTransform().GetPosition());
        SetUniform("point_light.diffuse", point_light_ptr->GetDiffuseColor());
        SetUniform("point_light.specular", point_light_ptr->GetSpecularColor());
        SetUniform("point_light.attenuation", point_light_ptr->GetAttenuation());
    }
    else {
        throw std::runtime_error(
            "Encountered light type unrecognized by the shader!");
    }
}

void PhongShader::AssociateVertexArray(VertexArray& InVertexArray) const
{
    if (!InVertexArray.HasPositionBuffer()) {
        throw std::runtime_error("Phong shader requires vertex positions!");
    }
    if (!InVertexArray.HasNormalBuffer()) {
        throw std::runtime_error("Phong shader requires vertex normals!");
    }

    InVertexArray.LinkPositionBuffer(GetAttributeLocation("vertex_position"));
    InVertexArray.LinkNormalBuffer(GetAttributeLocation("vertex_normal"));
    if (InVertexArray.HasTexCoordBuffer()) {
        InVertexArray.LinkTexCoordBuffer(GetAttributeLocation("vertex_tex_coord"));
    }
}

}