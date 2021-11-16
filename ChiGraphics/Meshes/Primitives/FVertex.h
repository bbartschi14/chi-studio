#pragma once
#include "ChiGraphics/External.h"

namespace CHISTUDIO {

class FHalfEdge;

class FVertex
{
public:
	FVertex(glm::vec3 InPosition, FHalfEdge* InParentHalfEdge, int InIndexId);

	void SetPosition(const glm::vec3& InPosition);
	glm::vec3 GetPosition() const;

	void SetParentHalfEdge(FHalfEdge* InParentHalfEdge);
	FHalfEdge* GetParentHalfEdge() const;

	void SetIndexId(int InIndexId);
	int GetIndexId() const;

	bool bMarkedForDeletion;
private:
	glm::vec3 Position;
	class FHalfEdge* ParentHalfEdge;
	int IndexId;
};

}

