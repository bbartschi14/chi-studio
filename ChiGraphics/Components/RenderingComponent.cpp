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
    bDisplayUnmodified = false;

    ShadingType = EShadingType::Flat;

    FDefaultObjectParams dummyParams;
    PostModifierVertexObj = std::make_shared<VertexObject>(EDefaultObject::CustomMesh, dummyParams);
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
    PostModifierVertexObj->GetVertexArray().SetDrawMode( InDrawMode );
}

EDrawMode RenderingComponent::GetDrawMode()
{
    return  VertexObj->GetVertexArray().GetDrawMode();
}

void RenderingComponent::SetPolygonMode(EPolygonMode InPolygonMode)
{
    VertexObj->GetVertexArray().SetPolygonMode( InPolygonMode );
    PostModifierVertexObj->GetVertexArray().SetPolygonMode( InPolygonMode );
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
    
    GetVertexObjectPtr()->Render();
}

void RenderingComponent::AddModifier(std::unique_ptr<IModifier> InModifier)
{
    Modifiers.push_back(std::move(InModifier));
    RecalculateModifiers();
}

void RenderingComponent::RecalculateModifiers()
{
    PostModifierVertexObj->CopyVertexObject(VertexObj.get());

    for (size_t i = 0; i < Modifiers.size(); i++)
    {
        Modifiers[i]->ApplyModifier(PostModifierVertexObj.get());
    }

    PostModifierVertexObj->MarkDirty();
}

void RenderingComponent::SetShadingType(EShadingType InShadingType)
{
    ShadingType = InShadingType;
    if (VertexObj) VertexObj->SetShadingType(ShadingType);
    if (PostModifierVertexObj) PostModifierVertexObj->SetShadingType(ShadingType);
}

void RenderingComponent::OnEnterEditMode()
{
    bDisplayUnmodified = true;
}

void RenderingComponent::OnExitEditMode()
{
    bDisplayUnmodified = false;
    RecalculateModifiers();
}

}