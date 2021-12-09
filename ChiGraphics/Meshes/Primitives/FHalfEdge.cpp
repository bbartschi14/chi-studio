#include "FHalfEdge.h"
#include "FFace.h"
#include "FVertex.h"
#include "FEdge.h"

namespace CHISTUDIO {

FHalfEdge::FHalfEdge(FHalfEdge* InNextHalfEdge, FHalfEdge* InSymmetricalHalfEdge, FFace* InOwningFace, FEdge* InOwningEdge, FVertex* InNextVertex, int InIndexId)
	: NextHalfEdge(InNextHalfEdge), SymmetricalHalfEdge(InSymmetricalHalfEdge), OwningFace(InOwningFace), OwningEdge(InOwningEdge), NextVertex(InNextVertex), IndexId(InIndexId), bMarkedForDeletion(false)
{
	if (NextHalfEdge)
	{
		NextHalfEdge->SetPreviousHalfEdge(this);
	}
}

void FHalfEdge::SetNextHalfEdge(FHalfEdge* InNextHalfEdge)
{
	NextHalfEdge = InNextHalfEdge;
	NextHalfEdge->SetPreviousHalfEdge(this);
}

FHalfEdge* FHalfEdge::GetNextHalfEdge() const
{
	return NextHalfEdge;
}

void FHalfEdge::SetPreviousHalfEdge(FHalfEdge* InPreviousHalfEdge)
{
	PreviousHalfEdge = InPreviousHalfEdge;
}

FHalfEdge* FHalfEdge::GetPreviousHalfEdge() const
{
	return PreviousHalfEdge;
}

void FHalfEdge::SetSymmetricalHalfEdge(FHalfEdge* InSymmetricalHalfEdge)
{
	SymmetricalHalfEdge = InSymmetricalHalfEdge;
}

FHalfEdge* FHalfEdge::GetSymmetricalHalfEdge() const
{
	return SymmetricalHalfEdge;
}

void FHalfEdge::SetOwningFace(FFace* InOwningFace)
{
	OwningFace = InOwningFace;
}

FFace* FHalfEdge::GetOwningFace() const
{
	return OwningFace;
}

void FHalfEdge::SetOwningEdge(FEdge* InOwningEdge)
{
	OwningEdge = InOwningEdge;;
}

FEdge* FHalfEdge::GetOwningEdge() const
{
	return OwningEdge;
}

void FHalfEdge::SetNextVertex(FVertex* InNextVertex)
{
	NextVertex = InNextVertex;
}

FVertex* FHalfEdge::GetNextVertex() const
{
	return NextVertex;
}

void FHalfEdge::SetIndexId(int InIndexId)
{
	IndexId = InIndexId;
}

int FHalfEdge::GetIndexId() const
{
	return IndexId;
}

bool FHalfEdge::HasNonDeletedFace() const
{
	return GetOwningFace() && !GetOwningFace()->bMarkedForDeletion;
}

}