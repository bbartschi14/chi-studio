#include "FVertex.h"
#include "FHalfEdge.h"
#include "FEdge.h"

namespace CHISTUDIO {

FVertex::FVertex(glm::vec3 InPosition, FHalfEdge* InParentHalfEdge, int InIndexId)
	: Position(InPosition), ParentHalfEdge(InParentHalfEdge), IndexId(InIndexId), bMarkedForDeletion(false)
{
}

void FVertex::SetPosition(const glm::vec3& InPosition)
{
	Position = InPosition;
}

glm::vec3 FVertex::GetPosition() const
{
	return Position;
}

void FVertex::SetParentHalfEdge(FHalfEdge* InParentHalfEdge)
{
	ParentHalfEdge = InParentHalfEdge;
}

FHalfEdge* FVertex::GetParentHalfEdge() const
{
	return ParentHalfEdge;
}

void FVertex::SetIndexId(int InIndexId)
{
	IndexId = InIndexId;
}

int FVertex::GetIndexId() const
{
	return IndexId;
}

void FVertex::SetPositionIndex(int InPositionIndex)
{
	PositionIndex = InPositionIndex;
}

int FVertex::GetPositionIndex() const
{
	return PositionIndex;
}

std::vector<FEdge*> FVertex::GetAdjacentEdges() const
{
	std::vector<FEdge*> adjacentEdges;

	FHalfEdge* firstHalfEdge = GetParentHalfEdge();
	FHalfEdge* nextHalfEdge = firstHalfEdge;
	do
	{
		adjacentEdges.push_back(nextHalfEdge->GetOwningEdge());
		nextHalfEdge = nextHalfEdge->GetNextHalfEdge()->GetSymmetricalHalfEdge();
	} while (nextHalfEdge != firstHalfEdge);

	return adjacentEdges;
}

void FVertex::GetAdjacentEdgesAndFaces(std::vector<class FEdge*>& OutAdjacentEdges, std::vector<class FFace*>& OutAdjacentFaces) const
{
	OutAdjacentEdges.clear();
	OutAdjacentFaces.clear();

	FHalfEdge* firstHalfEdge = GetParentHalfEdge();
	FHalfEdge* nextHalfEdge = firstHalfEdge;
	do
	{
		OutAdjacentEdges.push_back(nextHalfEdge->GetOwningEdge());
		if (FFace* owningFace = nextHalfEdge->GetOwningFace())
		{
			OutAdjacentFaces.push_back(owningFace);
		}
		nextHalfEdge = nextHalfEdge->GetNextHalfEdge()->GetSymmetricalHalfEdge();
	} while (nextHalfEdge != firstHalfEdge);
}

}