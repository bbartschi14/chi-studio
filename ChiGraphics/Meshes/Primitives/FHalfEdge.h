#pragma once

#include "ChiGraphics/External.h"

namespace CHISTUDIO {

class FVertex;
class FFace;
class FEdge;

class FHalfEdge
{
public:
	FHalfEdge(FHalfEdge* InNextHalfEdge, FHalfEdge* InSymmetricalHalfEdge, FFace* InOwningFace, FEdge* InOwningEdge, FVertex* InNextVertex, int InIndexId);

	void SetNextHalfEdge(FHalfEdge* InNextHalfEdge);
	FHalfEdge* GetNextHalfEdge() const;

	void SetPreviousHalfEdge(FHalfEdge* InPreviousHalfEdge);
	FHalfEdge* GetPreviousHalfEdge() const;

	void SetSymmetricalHalfEdge(FHalfEdge* InSymmetricalHalfEdge);
	FHalfEdge* GetSymmetricalHalfEdge() const;

	void SetOwningFace(FFace* InOwningFace);
	FFace* GetOwningFace() const;
	
	void SetOwningEdge(FEdge* InOwningEdge);
	FEdge* GetOwningEdge() const;

	void SetNextVertex(FVertex* InNextVertex);
	FVertex* GetNextVertex() const;

	void SetIndexId(int InIndexId);
	int GetIndexId() const;

	bool IsBoundaryHalfEdge() const { return OwningFace == nullptr; }
	bool HasNonDeletedFace() const;
	bool bMarkedForDeletion;
private:
	FHalfEdge* NextHalfEdge; // The next HalfEdge in the loop of HalfEdges that lie on this HalfEdge's Face
	FHalfEdge* PreviousHalfEdge;
	FHalfEdge* SymmetricalHalfEdge; // The HalfEdge that lies parallel to this HalfEdge and which travels in the opposite direction and is part of an adjacent Face
	FFace* OwningFace;
	FEdge* OwningEdge;

	FVertex* NextVertex;

	int IndexId;
};

}

