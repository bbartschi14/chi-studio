#pragma once

#include "ComponentBase.h"
#include "ChiGraphics/Meshes/VertexObject.h"
#include "ChiGraphics/Modifiers/Modifier.h"

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

    // Returns the appropriate VO based on modifiers and display flag
    VertexObject* GetVertexObjectPtr() const {
        if (bDisplayUnmodified || GetNumberOfModifiers() == 0)
        {
            return VertexObj.get();
        }
        else
        {
            return GetPostModifierVertexObjectPtr();
        }
    }
    
    // Returns the post-modifier vertex object
    VertexObject* GetPostModifierVertexObjectPtr() const {
        return PostModifierVertexObj.get();
    }
    
    // Returns the pre-modifier vertex object
    VertexObject* GetPreModifierVertexObjectPtr() const {
        return VertexObj.get();
    }

    void Render() const;

    bool bRenderSolid;
    bool bRenderWireframe;
    bool bRenderPoints;
    bool bIsDebugRender;
    bool bDisplayUnmodified;

    // Add a modifier to the list of modifiers on this rendering componenet. Immediately recalculates the post-modifier VO
    void AddModifier(std::unique_ptr<IModifier> InModifier, bool ShouldRecalculate = true);

    // Remove at an index
    void RemoveModifier(int InIndex);

    // Copy the main VO to the post-modifier VO, then apply all modifiers in order
    void RecalculateModifiers();

    void SetShadingType(EShadingType InShadingType);
    EShadingType GetShadingType() const { return ShadingType; }

    size_t GetNumberOfModifiers() const { return Modifiers.size(); }

    const std::vector<std::unique_ptr<IModifier>>& GetModifiers() const
    {
        return Modifiers;
    }

    void OnEnterEditMode();
    void OnExitEditMode();
private:
    std::shared_ptr<VertexObject> VertexObj;
    std::shared_ptr<VertexObject> PostModifierVertexObj;
    std::vector<std::unique_ptr<IModifier>> Modifiers;

    int StartIndex;
    int NumIndices;

    EShadingType ShadingType;
};

CREATE_COMPONENT_TRAIT(RenderingComponent, EComponentType::Rendering);

}
