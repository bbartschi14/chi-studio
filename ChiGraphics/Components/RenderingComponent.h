#pragma once

#include "ComponentBase.h"
#include "ChiGraphics/Meshes/VertexObject.h"

namespace CHISTUDIO {

class RenderingComponent : public ComponentBase 
{
public:
    RenderingComponent(std::shared_ptr<VertexObject> InVertexObj);
    void SetDrawRange(int InStartIndex, int InNumIndices);
    void SetVertexObject(std::shared_ptr<VertexObject> InVertexObj);
    void SetDrawMode(EDrawMode InDrawMode);
    EDrawMode GetDrawMode();
    void SetPolygonMode(EPolygonMode InPolygonMode);
    EPolygonMode GetPolygonMode();

    VertexObject* GetVertexObjectPtr() {
        return VertexObj.get();
    }

    void Render() const;

    bool bRenderSolid;
    bool bRenderWireframe;
    bool bRenderPoints;
private:
    std::shared_ptr<VertexObject> VertexObj;
    int StartIndex;
    int NumIndices;
};

CREATE_COMPONENT_TRAIT(RenderingComponent, EComponentType::Rendering);

}
