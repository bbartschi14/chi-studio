#pragma once
#include <string>

namespace CHISTUDIO {

    // Base class for any object modifiers. The rendering component stores a vector of these
    // to apply when needed.
    class IModifier
    {
    public:
        // Modifier logic function to be overriden. Should apply changes to the InObjectToModify
        virtual void ApplyModifier(class VertexObject* InObjectToModify) const = 0;

        // Custom UI element for the given modifier. Returns true if a modifier property was changed
        virtual bool RenderUI() = 0;
        virtual float GetUIHeight() const = 0;

        virtual std::string GetName() const = 0;

        virtual ~IModifier() {}
    };

}