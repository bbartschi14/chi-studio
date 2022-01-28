#include "FFace.h"
#include "FHalfEdge.h"
#include "FVertex.h"
#include "ChiGraphics/Collision/Hittables/TriangleHittable.h"

namespace CHISTUDIO {

	FFace::FFace(FHalfEdge* InHalfEdgeOnFace, int InIndexId)
		: HalfEdgeOnFace(InHalfEdgeOnFace), IndexId(InIndexId), bMarkedForDeletion(false)
	{
	}

	void FFace::SetHalfEdgeOnFace(FHalfEdge* InHalfEdgeOnFace)
	{
		HalfEdgeOnFace = InHalfEdgeOnFace;
	}

	FHalfEdge* FFace::GetHalfEdgeOnFace() const
	{
		return HalfEdgeOnFace;
	}

	void FFace::SetIndexId(int InIndexId)
	{
		IndexId = InIndexId;
	}

	int FFace::GetIndexId() const
	{
		return IndexId;
	}

	std::vector<FVertex*> FFace::GetVerticesOnFace() const
	{
		std::vector<FVertex*> verticesOnFace;
		FHalfEdge* startingHalfEdge = GetHalfEdgeOnFace();
		FHalfEdge* nextHalfEdge = startingHalfEdge;
		do 
		{
			verticesOnFace.push_back(nextHalfEdge->GetNextVertex());
			nextHalfEdge = nextHalfEdge->GetNextHalfEdge();
		} while (nextHalfEdge != startingHalfEdge);

		return verticesOnFace;
	}

	std::vector<FHalfEdge*> FFace::GetHalfEdgesOnFace() const
	{
		std::vector<FHalfEdge*> halfEdges;
		FHalfEdge* startingHalfEdge = HalfEdgeOnFace;
		FHalfEdge* nextHalfEdgeOnFace = startingHalfEdge;
		do
		{
			halfEdges.emplace_back(nextHalfEdgeOnFace);
			nextHalfEdgeOnFace = nextHalfEdgeOnFace->GetNextHalfEdge();
		} while (startingHalfEdge != nextHalfEdgeOnFace);
		return halfEdges;
	}

	glm::vec3 FFace::CalculateNaiveFaceNormal() const
	{
		std::vector<glm::vec3> positions;
		FHalfEdge* nextHalfEdge = GetHalfEdgeOnFace();
		for (int i = 0; i < 3; i++)
		{
			positions.emplace_back(nextHalfEdge->GetNextVertex()->GetPosition());
			nextHalfEdge = nextHalfEdge->GetNextHalfEdge();
		}
		glm::vec3 firstDirection = positions[1] - positions[0];
		glm::vec3 secondDirection = positions[2] - positions[0];
		glm::vec3 normal = glm::normalize(glm::cross(secondDirection, firstDirection));

		return normal;
	}

	std::vector<TriangleHittable> FFace::GetTrianglesOnFace() const
	{
		std::vector<TriangleHittable> triangles;
		std::vector<FVertex*> vertices = GetVerticesOnFace();
		glm::vec3 naiveNormal = CalculateNaiveFaceNormal();

		size_t numTriangles = vertices.size() - 2;
		for (size_t i = 0; i < numTriangles; i++)
		{
			triangles.emplace_back(vertices[0]->GetPosition(), vertices[i+1]->GetPosition(), vertices[i+2]->GetPosition(),
									naiveNormal, naiveNormal, naiveNormal, glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f));
		}

		return triangles;
	}

	glm::vec3 FFace::GetCentroid() const
	{
		std::vector<FVertex*> vertices = GetVerticesOnFace();

		glm::vec3 average = glm::vec3(0.0f);

		for (FVertex* vert : vertices)
		{
			average += vert->GetPosition();
		}

		return average / (float)vertices.size();
	}

	void FFace::SetImportedNormals(const std::vector<glm::vec3>& InNormals)
	{
		ImportedNormals.clear();
		for (glm::vec3 normal : InNormals)
		{
			ImportedNormals.push_back(normal);
		}
	}

	void FFace::SetUVs(const std::vector<glm::vec2>& InUVs)
	{
		UVs.clear();
		for (glm::vec2 uv : InUVs)
		{
			UVs.push_back(uv);
		}
	}
}
