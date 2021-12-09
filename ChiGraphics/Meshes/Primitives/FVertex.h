#pragma once
#include "ChiGraphics/External.h"
#include <vector>


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
	
	// Find all the adjacent edges to this vertex
	std::vector<class FEdge*> GetAdjacentEdges() const;

	// Find all the incoming half edges to this vertex
	std::vector<class FHalfEdge*> GetIncomingHalfEdges() const;

	// Find all the adjacent edges and faces. Slightly more optimized than doing both operations sequentially
	void GetAdjacentEdgesAndFaces(std::vector<class FEdge*>& OutAdjacentEdges, std::vector<class FFace*>& OutAdjacentFaces) const;

	bool bMarkedForDeletion;
private:
	glm::vec3 Position;
	class FHalfEdge* ParentHalfEdge;
	int IndexId;
	int PositionIndex;

};

}

