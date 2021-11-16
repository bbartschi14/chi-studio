#pragma once

#include "ShaderProgram.h"

#include "ChiGraphics/GL_Wrapper/FTexture.h"
#include "ChiGraphics/Meshes/VertexObject.h"

namespace CHISTUDIO {

class PlainTextureShader : public ShaderProgram {
 public:
  PlainTextureShader();

  void SetVertexObject(const VertexObject& obj) const;
  void SetTexture(const FTexture& texture, bool is_depth) const;

 private:
  void AssociateVertexArray(const VertexArray& vertex_array) const;
};

} 
