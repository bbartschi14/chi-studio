#pragma once

#include "ChiGraphics/External.h"

namespace CHISTUDIO {

class FHalfEdge;

/** Single edge represented by the two base half edges */
class FEdge
{
public:
	FEdge(FHalfEdge* InFirstHalfEdge, FHalfEdge* InSecondHalfEdge, int InIndexId);

	void SetFirstHalfEdge(FHalfEdge* InFirstHalfEdge);
	FHalfEdge* GetFirstHalfEdge() const;

	void SetSecondHalfEdge(FHalfEdge* InSecondHalfEdge);
	FHalfEdge* GetSecondHalfEdge() const;

	void SetIndexId(int InIndexId);
	int GetIndexId() const;

	bool bMarkedForDeletion;
private:
	FHalfEdge* FirstHalfEdge;
	FHalfEdge* SecondHalfEdge;

	int IndexId;
};

}
