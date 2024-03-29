#pragma once
#include "ChiGraphics/External.h"
#include <vector>
#include "ChiGraphics/Collision/Hittables/TriangleHittable.h"

namespace CHISTUDIO {

	class FHalfEdge;
	class FVertex;
	/** One of three primitive classes that make up a mesh data structure.
	 *  Does not store vertices of the face, instead traverses the mesh using a pointer
	 *  to a half-edge that lies on this face.
	 */
	class FFace
	{
	public:
		FFace(FHalfEdge* InHalfEdgeOnFace, int InIndexId);

		void SetHalfEdgeOnFace(FHalfEdge* InHalfEdgeOnFace);
		FHalfEdge* GetHalfEdgeOnFace() const;

		void SetIndexId(int InIndexId);
		int GetIndexId() const;

		std::vector<FVertex*> GetVerticesOnFace() const;
		std::vector<FHalfEdge*> GetHalfEdgesOnFace() const;

		/** Basic computation of the face normal, based purely off of the faces first three vertex positions.
		*	Used for mesh operations that just need a general direction, such as face extrusion, since normals
		*	aren't beign stored in the half-edge data structure.
		*/
		glm::vec3 CalculateNaiveFaceNormal() const;

		std::vector<TriangleHittable> GetTrianglesOnFace() const;

		/** Calculate and return the centroid based on the average of the vertices on this face */
		glm::vec3 GetCentroid() const;

		void SetImportedNormals(const std::vector<glm::vec3>& InNormals);
		const std::vector<glm::vec3>& GetImportedNormals() const { return ImportedNormals; }

		void SetUVs(const std::vector<glm::vec2>& InUVs);
		const std::vector<glm::vec2>& GetUVs() const { return UVs; }


		/** Flagged used for stages of primitive deletion operations */
		bool bMarkedForDeletion;
	private:
		FHalfEdge* HalfEdgeOnFace; // One of any number of half edges that line on this face
		int IndexId;
		std::vector<glm::vec3> ImportedNormals;
		std::vector<glm::vec2> UVs;
	};

}

