#include "FEdge.h"
#include "FHalfEdge.h"

namespace CHISTUDIO {

	FEdge::FEdge(FHalfEdge* InFirstHalfEdge, FHalfEdge* InSecondHalfEdge, int InIndexId)
		: FirstHalfEdge(InFirstHalfEdge), SecondHalfEdge(InSecondHalfEdge), IndexId(InIndexId), bMarkedForDeletion(false)
	{
		if (FirstHalfEdge)
		{
			FirstHalfEdge->SetOwningEdge(this);
		}

		if (SecondHalfEdge)
		{
			SecondHalfEdge->SetOwningEdge(this);
		}
	}

	void FEdge::SetFirstHalfEdge(FHalfEdge* InFirstHalfEdge)
	{
		FirstHalfEdge = InFirstHalfEdge;
		FirstHalfEdge->SetOwningEdge(this);
	}

	FHalfEdge* FEdge::GetFirstHalfEdge() const
	{
		return FirstHalfEdge;
	}

	void FEdge::SetSecondHalfEdge(FHalfEdge* InSecondHalfEdge)
	{
		SecondHalfEdge = InSecondHalfEdge;
		SecondHalfEdge->SetOwningEdge(this);
	}

	FHalfEdge* FEdge::GetSecondHalfEdge() const
	{
		return SecondHalfEdge;
	}

	void FEdge::SetIndexId(int InIndexId)
	{
		IndexId = InIndexId;
	}

	int FEdge::GetIndexId() const
	{
		return IndexId;
	}

}