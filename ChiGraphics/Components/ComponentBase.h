#pragma once

#include "ComponentType.h"

namespace CHISTUDIO {

class SceneNode;

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