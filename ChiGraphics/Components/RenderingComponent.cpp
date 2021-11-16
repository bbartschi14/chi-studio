#include "RenderingComponent.h"
#include <stdexcept>
#include <iostream>

namespace CHISTUDIO {

    RenderingComponent::RenderingComponent(std::shared_ptr<VertexObject> InVertexObj)
        : VertexObj(std::move(InVertexObj)), bIsDebugRender(false)
{
    if (!VertexObj->HasIndices() && !VertexObj->HasPositions()) {
        throw std::runtime_error(
            "Cannot initialize a "
            "RenderingComponent with a VertexObject without positions!");
    }

    // We use -1 to indicate the entire range of indices/positions.
    StartIndex = -1;
    NumIndices = -1;

    bRenderSolid = true;
    bRenderWireframe = false;
    bRenderPoints = false;
}

void RenderingComponent::SetDrawRange(int InStartIndex, int InNumIndices)
{
    StartIndex = InStartIndex;
    NumIndices = InNumIndices;
}

void RenderingComponent::SetVertexObject(std::shared_ptr<VertexObject> InVertexObj)
{
    VertexObj = InVertexObj;
}

void RenderingComponent::SetDrawMode(EDrawMode InDrawMode)
{
    VertexObj->GetVertexArray().SetDrawMode( InDrawMode );
}

EDrawMode RenderingComponent::GetDrawMode()
{
    return  VertexObj->GetVertexArray().GetDrawMode();
}

void RenderingComponent::SetPolygonMode(EPolygonMode InPolygonMode)
{
    VertexObj->GetVertexArray().SetPolygonMode( InPolygonMode );
}

EPolygonMode RenderingComponent::GetPolygonMode()
{
    return VertexObj->GetVertexArray().GetPolygonMode();
}

void RenderingComponent::Render() const
{
    if (VertexObj == nullptr) {
        throw std::runtime_error(
            "Rendering component has no vertex object attached!");
    }
    //if (StartIndex >= 0 && NumIndices > 0) {
    //    VertexObj->GetVertexArray().Render(static_cast<size_t>(StartIndex),
    //        static_cast<size_t>(NumIndices));
    //}
    
    VertexObj->Render();
    
}

}