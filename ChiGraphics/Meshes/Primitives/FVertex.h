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

	// Index Id is used for unique identification
	void SetIndexId(int InIndexId);
	int GetIndexId() const;

	// Position Index is used for storing this vertex's position within a vertex position array
	void SetPositionIndex(int InPositionIndex);
	int GetPositionIndex() const;

	bool bMarkedForDeletion;
private:
	glm::vec3 Position;
	class FHalfEdge* ParentHalfEdge;
	int IndexId;
	int PositionIndex;

};

}

