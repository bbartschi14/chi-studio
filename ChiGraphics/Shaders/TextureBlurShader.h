#pragma once

#include "ShaderProgram.h"

#include "ChiGraphics/GL_Wrapper/FTexture.h"
#include "ChiGraphics/Meshes/VertexObject.h"

namespace CHISTUDIO {

class TextureBlurShader : public ShaderProgram {
 public:
	 TextureBlurShader();

  void SetVertexObject(const VertexObject& obj) const;
  void SetTexture(const FTexture& texture) const;

 private:
  void AssociateVertexArray(const VertexArray& vertex_array) const;
};

} 
