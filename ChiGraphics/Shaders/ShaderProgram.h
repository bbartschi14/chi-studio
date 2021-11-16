#pragma once

#include "ChiGraphics/GL_Wrapper/IBindable.h"
#include "ChiGraphics/GL_Wrapper/VertexArray.h"
#include "ChiGraphics/Transform.h"

#include <string>
#include <unordered_map>
#include <glad/glad.h>

namespace CHISTUDIO {

class CameraComponent;
class LightComponent;
class SceneNode;

class ShaderProgram : public IBindable 
{
public:
    ShaderProgram(const std::unordered_map<GLenum, std::string>& InShaderFilenames);
    virtual ~ShaderProgram();

    void Bind() const override;
    void Unbind() const override;

    GLint GetAttributeLocation(const std::string& InName) const;

    // The following Set* methods are called by the renderer, thus const.
    virtual void SetTargetNode(const SceneNode& InNode, const glm::mat4& InLocalToWordMatrix) const {}
    virtual void SetCamera(const CameraComponent& InCamera) const {}
    virtual void SetLightSource(const LightComponent& InLight) const {}

protected:
    // Protected because only shader subclasses have information to the names.
    void SetUniform(const std::string& InName, const glm::mat4& InValue) const;
    void SetUniform(const std::string& InName, const glm::mat3& InValue) const;
    void SetUniform(const std::string& InName, const glm::vec3& InValue) const;
    void SetUniform(const std::string& InName, float InValue) const;
    void SetUniform(const std::string& InName, int InValue) const;

private:
    static GLuint LoadShader(GLenum InType, std::string InShaderCode, const std::string& InShaderFilename);
    const static int kErrorLogBufferSize = 512;

    std::unordered_map<GLenum, GLuint> ShaderHandles;
    GLuint ShaderProgram_;
};

}
