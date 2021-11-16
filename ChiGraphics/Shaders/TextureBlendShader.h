#pragma once

#include "ShaderProgram.h"

#include "ChiGraphics/GL_Wrapper/FTexture.h"
#include "ChiGraphics/Meshes/VertexObject.h"

namespace CHISTUDIO {

class TextureBlendShader : public ShaderProgram {
 public:
	 TextureBlendShader();

  void SetVertexObject(const VertexObject& obj) const;
  void SetTextures(const FTexture& texture1, const FTexture& texture2) const;

 private:
  void AssociateVertexArray(const VertexArray& vertex_array) const;
};

} 
