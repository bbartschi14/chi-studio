#include "TextureBlendShader.h"

#include "ChiGraphics/SceneNode.h"
#include "ChiGraphics/Components/RenderingComponent.h"

namespace CHISTUDIO {

    TextureBlendShader::TextureBlendShader()
    : ShaderProgram(std::unordered_map<GLenum, std::string>{
          {GL_VERTEX_SHADER, "texture_blend.vert"},
          {GL_FRAGMENT_SHADER, "texture_blend.frag"}}) {
}

void TextureBlendShader::AssociateVertexArray(
    const VertexArray& vertex_array) const 
{
  if (!vertex_array.HasPositionBuffer()) {
    throw std::runtime_error("texture shader requires vertex positions!");
  }
  if (!vertex_array.HasTexCoordBuffer()) {
    throw std::runtime_error(
        "texture shader requires vertex texture coordinates!");
  }
  vertex_array.LinkPositionBuffer(GetAttributeLocation("vertex_ndc_position"));
  vertex_array.LinkTexCoordBuffer(GetAttributeLocation("vertex_tex_coord"));
}

void TextureBlendShader::SetVertexObject(const VertexObject& obj) const 
{
  AssociateVertexArray(obj.GetVertexArray());
}

void TextureBlendShader::SetTextures(const FTexture& texture1, const FTexture& texture2) const 
{
  texture1.BindToUnit(0);
  SetUniform("in_texture1", 0);

  texture2.BindToUnit(1);
  SetUniform("in_texture2", 1);
}

}
