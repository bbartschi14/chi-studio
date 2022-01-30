#pragma once

#include "ComponentType.h"

namespace CHISTUDIO {

class SceneNode;

/** ComponentBase is the base class for components that make up scene nodes. 
 * Components store references to their owning node.
 */
class ComponentBase 
{
public:
    virtual ~ComponentBase() {}

    void SetNodePtr(SceneNode* InNodePtr) {
        NodePtr = InNodePtr;
    }

    SceneNode* GetNodePtr() const {
        return NodePtr;
    }

protected:
    SceneNode* NodePtr;
};

}