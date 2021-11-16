#include "TextureBlurShader.h"

#include "ChiGraphics/SceneNode.h"
#include "ChiGraphics/Components/RenderingComponent.h"

namespace CHISTUDIO {

    TextureBlurShader::TextureBlurShader()
    : ShaderProgram(std::unordered_map<GLenum, std::string>{
          {GL_VERTEX_SHADER, "texture_blur.vert"},
          {GL_FRAGMENT_SHADER, "texture_blur.frag"}}) {
}

void TextureBlurShader::AssociateVertexArray(
    const VertexArray& vertex_array) const {
  if (!vertex_array.HasPositionBuffer()) {
    throw std::runtime_error("Plain texture shader requires vertex positions!");
  }
  if (!vertex_array.HasTexCoordBuffer()) {
    throw std::runtime_error(
        "Plain texture shader requires vertex texture coordinates!");
  }
  vertex_array.LinkPositionBuffer(GetAttributeLocation("vertex_ndc_position"));
  vertex_array.LinkTexCoordBuffer(GetAttributeLocation("vertex_tex_coord"));
}

void TextureBlurShader::SetVertexObject(const VertexObject& obj) const {
  AssociateVertexArray(obj.GetVertexArray());
}

void TextureBlurShader::SetTexture(const FTexture& texture ) const {
  texture.BindToUnit(0);
  SetUniform("in_texture", 0);
  SetUniform("width", (float)texture.Width);
  SetUniform("height", (float)texture.Height);

}

}
