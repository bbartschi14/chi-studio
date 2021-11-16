#include "EdgeShader.h"

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

    EdgeShader::EdgeShader()
        : ShaderProgram(std::unordered_map<GLenum, std::string>
    {
        {GL_VERTEX_SHADER, "edge.vert"},
        { GL_FRAGMENT_SHADER, "edge.frag" }
    })
    {
    }

        void EdgeShader::SetTargetNode(const SceneNode& InNode, const glm::mat4& InModelMatrix) const
        {
            // Associate the right VAO before rendering.
            AssociateVertexArray(InNode.GetComponentPtr<RenderingComponent>()->GetVertexObjectPtr()->GetVertexArray());

            // Set transform.
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(InModelMatrix)));
            SetUniform("model_matrix", InModelMatrix);
        }

        void EdgeShader::SetCamera(const CameraComponent& InCamera) const
        {
            SetUniform("view_matrix", InCamera.GetViewMatrix());
            SetUniform("projection_matrix", InCamera.GetProjectionMatrix());
            SetUniform("camera_position", InCamera.GetAbsoluteCameraPosition());
        }

        void EdgeShader::AssociateVertexArray(const VertexArray& InVertexArray) const
        {
            if (!InVertexArray.HasPositionBuffer()) {
                throw std::runtime_error("Edge shader requires vertex positions!");
            }
           
            InVertexArray.LinkPositionBuffer(GetAttributeLocation("vertex_position"));
        }

        void EdgeShader::SetSelectedStyle(bool bIsSelected) const
        {
            SetUniform("isSelected", bIsSelected ? 1.0f : 0.0f);
        }


}
