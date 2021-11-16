#include "FVertex.h"
#include "FHalfEdge.h"

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

}