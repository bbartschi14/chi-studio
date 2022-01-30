#pragma once
#include "ComponentBase.h"
#include "ChiGraphics/Shaders/ShaderProgram.h"
#include "ChiGraphics/AliasTypes.h"
#include <memory>

namespace CHISTUDIO {

/** Stores a shader to be used for the viewport renderer. */
class ShadingComponent : public ComponentBase 
{
public:
    ShadingComponent(std::shared_ptr<ShaderProgram> InShader)
        : Shader(std::move(InShader)) {
    }
    ShaderProgram* GetShaderPtr() {
        return Shader.get();
    }

private:
    std::shared_ptr<ShaderProgram> Shader;
};

CREATE_COMPONENT_TRAIT(ShadingComponent, EComponentType::Shading);

}
