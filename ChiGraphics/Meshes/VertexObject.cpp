#include "VertexObject.h"
#include "ChiGraphics/GL_Wrapper/BindGuard.h"
#include "ChiGraphics/SceneNode.h"
#include "ChiGraphics/Shaders/PointShader.h"
#include "ChiGraphics/Collision/FRay.h"
#include "ChiGraphics/Collision/FHitRecord.h"
#include "ChiGraphics/Collision/Hittables/SphereHittable.h"
#include "ChiGraphics/Collision/Hittables/CylinderHittable.h"
#include "ChiGraphics/Collision/Hittables/TriangleHittable.h"

#include <memory>
#include <iostream>
#include <stdexcept>
#include <functional>
#include "ChiGraphics/Application.h"

namespace CHISTUDIO {

	void VertexObject::UpdatePositions(std::unique_ptr<FPositionArray> InPositions)
	{
		if (Positions == nullptr) {
			VertexArray_->CreatePositionBuffer();
		}
		Positions = std::move(InPositions);
		VertexArray_->UpdatePositions(*Positions);
	}

	void VertexObject::UpdateNormals(std::unique_ptr<FNormalArray> InNormals)
	{
		if (Normals == nullptr) {
			VertexArray_->CreateNormalBuffer();
		}
		Normals = std::move(InNormals);
		VertexArray_->UpdateNormals(*Normals);
	}

	void VertexObject::UpdateColors(std::unique_ptr<FColorArray> InColors)
	{
		if (Colors == nullptr) {
			VertexArray_->CreateColorBuffer();
		}
		Colors = std::move(InColors);
		VertexArray_->UpdateColors(*Colors);
	}

	void VertexObject::UpdateTexCoord(std::unique_ptr<FTexCoordArray> InTexCoords)
	{
		if (TexCoords == nullptr) {
			VertexArray_->CreateTexCoordBuffer();
		}
		TexCoords = std::move(InTexCoords);
		VertexArray_->UpdateTexCoords(*TexCoords);
	}

	void VertexObject::UpdateIndices(std::unique_ptr<FIndexArray> InIndices)
	{
		if (Indices == nullptr) {
			VertexArray_->CreateIndexBuffer();
		}
		Indices = std::move(InIndices);
		VertexArray_->UpdateIndices(*Indices);
	}

	void VertexObject::CreateHalfEdgeCube()
	{
		std::set<int> usedHalfEdgeIndices;
		int edgeIndex = 0;
		auto SetupFace = [&](int faceIndex, int halfEdgeIndices[4], int vertexIndices[4], int symmetricalEdgeIndices[4])
		{
			Faces[faceIndex].get()->SetHalfEdgeOnFace(HalfEdges[halfEdgeIndices[0]].get());

			for (int i = 0; i < 4; i++)
			{
				FHalfEdge* halfEdge = HalfEdges[halfEdgeIndices[i]].get();
				halfEdge->SetNextHalfEdge(HalfEdges[halfEdgeIndices[(i+1)%4]].get());
				halfEdge->SetNextVertex(Vertices[vertexIndices[i]].get());
				Vertices[vertexIndices[i]].get()->SetParentHalfEdge(halfEdge);
				halfEdge->SetSymmetricalHalfEdge(HalfEdges[symmetricalEdgeIndices[i]].get());
				halfEdge->SetOwningFace(Faces[faceIndex].get());

				// Create edge
				if (usedHalfEdgeIndices.find(halfEdge->GetIndexId()) == usedHalfEdgeIndices.end())
				{
					FEdge* newEdge = CreateEdge(halfEdge, halfEdge->GetSymmetricalHalfEdge());
					halfEdge->SetOwningEdge(newEdge);
					halfEdge->GetSymmetricalHalfEdge()->SetOwningEdge(newEdge);

					// Keep track of used half edges
					usedHalfEdgeIndices.insert(halfEdge->GetIndexId());
					usedHalfEdgeIndices.insert(halfEdge->GetSymmetricalHalfEdge()->GetIndexId());
				}

			}
		};

		// Construct faces, halfedges, and vertices
		int NUM_FACES = 6;
		for (int i = 0; i < NUM_FACES; i++)
		{
			CreateFace(nullptr);
		}

		int NUM_HALFEDGES = NUM_FACES * 4;
		for (int i = 0; i < NUM_HALFEDGES; i++)
		{
			CreateHalfEdge(nullptr, nullptr, nullptr, nullptr, nullptr);
		}
		
		CreateVertex(glm::vec3(1.0f, 1.0f, -1.0f), nullptr);
		CreateVertex(glm::vec3(-1.0f, 1.0f, -1.0f), nullptr);
		CreateVertex(glm::vec3(-1.0f, -1.0f, -1.0f), nullptr);
		CreateVertex(glm::vec3(1.0f, -1.0f, -1.0f), nullptr);
		CreateVertex(glm::vec3(-1.0f, 1.0f, 1.0f), nullptr);
		CreateVertex(glm::vec3(1.0f, 1.0f, 1.0f), nullptr);
		CreateVertex(glm::vec3(1.0f, -1.0f, 1.0f), nullptr);
		CreateVertex(glm::vec3(-1.0f, -1.0f, 1.0f), nullptr);

		// Connect all mesh primitive structs

		// FRONT FACE
		int edgesFront[] = { 0, 1, 2, 3 };
		int vertsFront[] = { 0, 1, 2, 3 };
		int symEdgesFront[] = { 4, 8, 12, 16 };
		SetupFace(0, edgesFront, vertsFront, symEdgesFront);

		// RIGHT FACE
		int edgesRight[] = { 4, 5, 6 ,7 };
		int vertsRight[] = { 3, 6, 5, 0 };
		int symEdgesRight[] = { 0, 19, 22, 9 };
		SetupFace(1, edgesRight, vertsRight, symEdgesRight);

		// TOP FACE
		int	   edgesTop[] = { 8, 9, 10, 11 };
		int	   vertsTop[] = { 0, 5, 4, 1 };
		int symEdgesTop[] = { 1, 7, 21, 13 };
		SetupFace(2, edgesTop,
					 vertsTop,
			      symEdgesTop);

		// LEFT FACE
		   int edgesLeft[] = { 12, 13, 14, 15 };
		   int vertsLeft[] = { 1, 4, 7, 2 };
		int symEdgesLeft[] = { 2, 11, 20, 17 };
		SetupFace(3, edgesLeft, 
					 vertsLeft, 
				  symEdgesLeft);

		// BOTTOM FACE
		int	   edgesBottom[] = { 16, 17, 18, 19 };
		int	   vertsBottom[] = { 2, 7, 6, 3 };
		int symEdgesBottom[] = { 3, 15, 23, 5 };
		SetupFace(4, edgesBottom,
					 vertsBottom,
			      symEdgesBottom);
		
		// BACK FACE
		int	   edgesBack[] = { 20, 21, 22, 23 };
		int	   vertsBack[] = { 4, 5, 6, 7 };
		int symEdgesBack[] = { 14, 10, 6, 18 };
		SetupFace(5, edgesBack,
					 vertsBack,
			      symEdgesBack);
	}

	void VertexObject::CreateHalfEdgePlane()
	{
		FFace* planeFace = CreateFace(nullptr);

		// Inner loop
		FHalfEdge* halfEdge1 = CreateHalfEdge(nullptr, nullptr, planeFace, nullptr, nullptr);
		FHalfEdge* halfEdge2 = CreateHalfEdge(nullptr, nullptr, planeFace, nullptr, nullptr);
		FHalfEdge* halfEdge3 = CreateHalfEdge(nullptr, nullptr, planeFace, nullptr, nullptr);
		FHalfEdge* halfEdge4 = CreateHalfEdge(nullptr, nullptr, planeFace, nullptr, nullptr);

		// Outer loop
		FHalfEdge* halfEdge5 = CreateHalfEdge(nullptr, nullptr, nullptr, nullptr, nullptr);
		FHalfEdge* halfEdge6 = CreateHalfEdge(nullptr, nullptr, nullptr, nullptr, nullptr);
		FHalfEdge* halfEdge7 = CreateHalfEdge(nullptr, nullptr, nullptr, nullptr, nullptr);
		FHalfEdge* halfEdge8 = CreateHalfEdge(nullptr, nullptr, nullptr, nullptr, nullptr);

		// Edges
		FEdge* edge1 = CreateEdge(halfEdge1, halfEdge5);
		FEdge* edge2 = CreateEdge(halfEdge2, halfEdge6);
		FEdge* edge3 = CreateEdge(halfEdge3, halfEdge7);
		FEdge* edge4 = CreateEdge(halfEdge4, halfEdge8);

		// Vertices
		FVertex* vertex1 = CreateVertex(glm::vec3(1.0f, 0.0f, 1.0f), halfEdge1);
		FVertex* vertex2 = CreateVertex(glm::vec3(1.0f, 0.0f, -1.0f), halfEdge2);
		FVertex* vertex3 = CreateVertex(glm::vec3(-1.0f, 0.0f, -1.0f), halfEdge3);
		FVertex* vertex4 = CreateVertex(glm::vec3(-1.0f, 0.0f, 1.0f), halfEdge4);

		// Finish Face
		planeFace->SetHalfEdgeOnFace(halfEdge1);

		// Finish Inner Loop
		halfEdge1->SetNextHalfEdge(halfEdge2);
		halfEdge2->SetNextHalfEdge(halfEdge3);
		halfEdge3->SetNextHalfEdge(halfEdge4);
		halfEdge4->SetNextHalfEdge(halfEdge1);

		halfEdge1->SetNextVertex(vertex1);
		halfEdge2->SetNextVertex(vertex2);
		halfEdge3->SetNextVertex(vertex3);
		halfEdge4->SetNextVertex(vertex4);

		halfEdge1->SetSymmetricalHalfEdge(halfEdge5);
		halfEdge2->SetSymmetricalHalfEdge(halfEdge6);
		halfEdge3->SetSymmetricalHalfEdge(halfEdge7);
		halfEdge4->SetSymmetricalHalfEdge(halfEdge8);

		// Finish Outer Loop
		halfEdge5->SetNextHalfEdge(halfEdge6);
		halfEdge6->SetNextHalfEdge(halfEdge7);
		halfEdge7->SetNextHalfEdge(halfEdge8);
		halfEdge8->SetNextHalfEdge(halfEdge5);

		halfEdge5->SetNextVertex(vertex4);
		halfEdge6->SetNextVertex(vertex3);
		halfEdge7->SetNextVertex(vertex2);
		halfEdge8->SetNextVertex(vertex1);

		halfEdge5->SetSymmetricalHalfEdge(halfEdge1);
		halfEdge6->SetSymmetricalHalfEdge(halfEdge2);
		halfEdge7->SetSymmetricalHalfEdge(halfEdge3);
		halfEdge8->SetSymmetricalHalfEdge(halfEdge4);
	}

	void VertexObject::CreateVertexArrayFromHalfEdgeStructure()
	{
		// Iterates over all the faces, triangulating them and creating
		// position, normal, and index arrays for the VBO.
		// Also creates a position array for the edges to draw a non-tesselated wireframe

		auto newPositions = make_unique<FPositionArray>();
		auto newNormals = make_unique<FNormalArray>();
		auto newIndices = make_unique<FIndexArray>();

		auto newWireframePositions = make_unique<FPositionArray>();
		
		unsigned int currentVertIndex = 0;
		for (int i = 0; i < Faces.size(); i++)
		{
			if (i == 6)
			{
				int i = 0;
			}
			FHalfEdge* startingHalfEdge = Faces[i].get()->GetHalfEdgeOnFace();
			FVertex* firstVertex = startingHalfEdge->GetNextVertex();
			newPositions->push_back(firstVertex->GetPosition());
			newWireframePositions->push_back(firstVertex->GetPosition());

			unsigned int firstVertexIndex = currentVertIndex;
			currentVertIndex++;

			FHalfEdge* nextHalfEdge = startingHalfEdge->GetNextHalfEdge();
			std::vector<unsigned int> vertexIndices;
			while (startingHalfEdge != nextHalfEdge)
			{
				FVertex* nextVertex = nextHalfEdge->GetNextVertex();
				newPositions->push_back(nextVertex->GetPosition());
				newWireframePositions->push_back(nextVertex->GetPosition());
				newWireframePositions->push_back(nextVertex->GetPosition());
				vertexIndices.push_back(currentVertIndex);
				currentVertIndex++;

				if (vertexIndices.size() == 2)
				{
					// Add triangle with first vertex and 2 vertices found
					newIndices->push_back(firstVertexIndex);
					newIndices->push_back(vertexIndices[0]);
					newIndices->push_back(vertexIndices[1]);

					vertexIndices[0] = vertexIndices[1]; // Shift over vertex
					vertexIndices.pop_back();
				}

				nextHalfEdge = nextHalfEdge->GetNextHalfEdge();
			}
			newWireframePositions->push_back(firstVertex->GetPosition());

			// Add normals
			std::vector<glm::vec3>& positionsRef = *newPositions;
			glm::vec3 firstDirection = positionsRef[firstVertexIndex + 1] - positionsRef[firstVertexIndex];
			glm::vec3 secondDirection = positionsRef[firstVertexIndex + 2] - positionsRef[firstVertexIndex];
			glm::vec3 normal = glm::normalize(glm::cross(secondDirection, firstDirection));
			for (unsigned int i = 0; i < currentVertIndex - firstVertexIndex; i++)
			{
				newNormals->push_back(normal);
			}
		}

		UpdatePositions(std::move(newPositions));
		UpdateIndices(std::move(newIndices));
		UpdateNormals(std::move(newNormals));

		EdgeVertexArray_->UpdatePositions(*std::move(newWireframePositions));

		OnSelectionChanged();

		VerifyHalfEdgeStability();
	}

	std::vector<std::unique_ptr<FFaceRegion>> VertexObject::FindRegionsFromFaces(std::vector<FFace*> InFaces) const
	{
		// High Level: General idea is a flood fill type algorithm.
		//	- Start with the first face.
		//	- Check all face neighbors. If any are in the InFaces vector and haven't been assigned yet, repeat this step on them.
		//		If a neighbor is not in InFaces, the half-edge belonging to the current face that is between the two faces should be assigned to the boundary loop.
		//  - Once no more neighbors belong to the InFaces vector, continue iterating through the InFaces array.
		//		If we find a face that hasn't been assign a region yet, repeat this algorithm on it.
		// By the end, we should have assigned all InFaces to a region, and each region should have an unsorted vector of boundary loop half edges.
		// After that, we take all the unsorted boundary edges and sort it into contiguous boundary loops, similarly to how we flood filled the faces.

		std::set<int> usableFaceIDs;
		std::set<int> usedFaceIDs;

		for (FFace* face : InFaces)
		{
			usableFaceIDs.insert(face->GetIndexId());
		}

		std::vector<std::unique_ptr<FFaceRegion>> regions;

		std::function<void(FFace*, FFaceRegion*)> RecursiveFloodFill = [&](FFace* InStartingFace, FFaceRegion* InCurrentRegion)
		{
			InCurrentRegion->FacesInRegion.emplace_back(InStartingFace);
			usableFaceIDs.erase(InStartingFace->GetIndexId());
			usedFaceIDs.insert(InStartingFace->GetIndexId());

			for (FHalfEdge* halfEdge : InStartingFace->GetHalfEdgesOnFace())
			{
				FFace* neighborFace = halfEdge->GetSymmetricalHalfEdge()->GetOwningFace();
				if (neighborFace && !(usableFaceIDs.find(neighborFace->GetIndexId()) == usableFaceIDs.end()))
				{
					RecursiveFloodFill(neighborFace, InCurrentRegion);
				}
				else if (neighborFace && (usedFaceIDs.find(neighborFace->GetIndexId()) == usedFaceIDs.end()))
				{
					// Only add edges if the face hasn't been use used and is not usable
					InCurrentRegion->UnsortedLoopEdges.emplace_back(halfEdge);
				}
			}
		};

		for (FFace* face : InFaces)
		{
			if (!(usableFaceIDs.find(face->GetIndexId()) == usableFaceIDs.end()))
			{
				std::unique_ptr<FFaceRegion> currentRegion = make_unique<FFaceRegion>();
				RecursiveFloodFill(face, currentRegion.get());
				SortRegionBoundaryLoops(currentRegion.get());
				regions.emplace_back(std::move(currentRegion));
			}
		}

		return regions;
	}

	void VertexObject::SortRegionBoundaryLoops(FFaceRegion* InFaceRegion) const
	{
		std::set<int> usableHalfEdgeIDs;
		std::set<int> usableFaceIDs;

		for (FFace* face : InFaceRegion->FacesInRegion)
		{
			usableFaceIDs.insert(face->GetIndexId());
		}

		std::function<FHalfEdge* (FHalfEdge* InHalfEdge)> GetNextHalfEdgeInLoop = [&](FHalfEdge* InHalfEdge) -> FHalfEdge* // Explicit return type annotation allows us to return nullptr
		{
			FHalfEdge* startingHalfEdge = InHalfEdge->GetNextHalfEdge();
			FHalfEdge* nextIncidentHalfEdge = startingHalfEdge;
			do
			{
				// Check if current edge is the next edge in the loop
				if (!(usableHalfEdgeIDs.find(nextIncidentHalfEdge->GetIndexId()) == usableHalfEdgeIDs.end()))
				{
					return nextIncidentHalfEdge;
				}

				nextIncidentHalfEdge = nextIncidentHalfEdge->GetSymmetricalHalfEdge()->GetNextHalfEdge();

			} while (nextIncidentHalfEdge != startingHalfEdge);

			return nullptr;
		};

		std::function<FIncidentHalfEdges(FHalfEdge* InStartingEdge)> GetIncidentHalfEdgesInRegion = [&](FHalfEdge* InStartingEdge)
		{

			FIncidentHalfEdges incidentHalfEdges;
			FHalfEdge* nextIncidentHalfEdge = InStartingEdge->GetNextHalfEdge()->GetSymmetricalHalfEdge();
			do
			{
				if (usableFaceIDs.find(nextIncidentHalfEdge->GetOwningFace()->GetIndexId()) != usableFaceIDs.end())
				{
					incidentHalfEdges.IncidentHalfEdges.emplace_back(nextIncidentHalfEdge);
				}
				nextIncidentHalfEdge = nextIncidentHalfEdge->GetNextHalfEdge()->GetSymmetricalHalfEdge();
			} while (nextIncidentHalfEdge != InStartingEdge);
			return incidentHalfEdges;
		};

		for (FHalfEdge* halfEdge : InFaceRegion->UnsortedLoopEdges)
		{
			usableHalfEdgeIDs.insert(halfEdge->GetIndexId());
		}

		for (FHalfEdge* halfEdge : InFaceRegion->UnsortedLoopEdges)
		{
			if (!(usableHalfEdgeIDs.find(halfEdge->GetIndexId()) == usableHalfEdgeIDs.end()))
			{
				std::shared_ptr<FBoundaryLoop> currentLoop = std::make_shared<FBoundaryLoop>();
				FHalfEdge* startingEdge = halfEdge;
				FHalfEdge* nextHalfEdgeInLoop = startingEdge;
				do
				{
					usableHalfEdgeIDs.erase(nextHalfEdgeInLoop->GetIndexId());
					currentLoop->Loop.emplace_back(nextHalfEdgeInLoop);

					// Find incident half edges to this half edges vertex that are within the region
					currentLoop->IncidentHalfEdgesPerLoopEdge.emplace_back(GetIncidentHalfEdgesInRegion(nextHalfEdgeInLoop));

					nextHalfEdgeInLoop = GetNextHalfEdgeInLoop(nextHalfEdgeInLoop);
				} while (nextHalfEdgeInLoop && startingEdge != nextHalfEdgeInLoop);

			InFaceRegion->BoundaryLoops.emplace_back(std::move(currentLoop));
			}
		}
	}

	void VertexObject::VerifyHalfEdgeStability()
	{
		std::string baseMessage = "Invalid Half-Mesh Prim: ";

		for (size_t i = 0; i < Faces.size(); i++)
		{
			FFace* currentFace = Faces[i].get();

			std::string baseFaceMessage = fmt::format("Face {}: ", currentFace->GetIndexId());

			if (currentFace->GetHalfEdgeOnFace() == nullptr)
			{
				std::cout << baseMessage << baseFaceMessage << "Null half edge" << std::endl;
			}
		}

		for (size_t i = 0; i < HalfEdges.size(); i++)
		{
			FHalfEdge* currentHalfEdge = HalfEdges[i].get();

			std::string baseHalfEdgeMessage = fmt::format("Half Edge {}: ", currentHalfEdge->GetIndexId());

			if (currentHalfEdge->GetNextHalfEdge() == nullptr)
			{
				std::cout << baseMessage << baseHalfEdgeMessage << "Null next half edge" << std::endl;
			}
			if (currentHalfEdge->GetPreviousHalfEdge() == nullptr)
			{
				std::cout << baseMessage << baseHalfEdgeMessage << "Null previous half edge" << std::endl;
			}
			if (currentHalfEdge->GetNextVertex() == nullptr)
			{
				std::cout << baseMessage << baseHalfEdgeMessage << "Null next vertex" << std::endl;
			}
			if (currentHalfEdge->GetOwningFace() == nullptr)
			{
				std::cout << baseMessage << baseHalfEdgeMessage << "Null owning face. Is this a mesh boundary half edge?" << std::endl;
			}
			if (currentHalfEdge->GetOwningEdge() == nullptr)
			{
				std::cout << baseMessage << baseHalfEdgeMessage << "Null owning edge" << std::endl;
			}
			if (currentHalfEdge->GetSymmetricalHalfEdge() == nullptr)
			{
				std::cout << baseMessage << baseHalfEdgeMessage << "Null symmetrical half edge" << std::endl;
			}
		}

		for (size_t i = 0; i < Edges.size(); i++)
		{
			FEdge* currentEdge = Edges[i].get();

			std::string baseEdgeMessage = fmt::format("Edge {}: ", currentEdge->GetIndexId()); 
			if (currentEdge->GetFirstHalfEdge() == nullptr)
			{
				std::cout << baseMessage << baseEdgeMessage << "Null first half edge" << std::endl;
			}
			if (currentEdge->GetSecondHalfEdge() == nullptr)
			{
				std::cout << baseMessage << baseEdgeMessage << "Null second half edge" << std::endl;
			}
		}

		for (size_t i = 0; i < Vertices.size(); i++)
		{
			FVertex* currentVertex = Vertices[i].get();

			std::string baseVertexMessage = fmt::format("Vertex {}: ", currentVertex->GetIndexId());
			if (currentVertex->GetParentHalfEdge() == nullptr)
			{
				std::cout << baseMessage << baseVertexMessage << "Null parent half edge" << std::endl;
			}
			else if (currentVertex->GetParentHalfEdge()->GetNextVertex() != currentVertex)
			{
				std::cout << baseMessage << baseVertexMessage << "Parent of vertex doesn't point to it" << std::endl;
			}
		}
	}

	void VertexObject::SelectVertex(int InIndex, bool addToSelection)
	{
		if (!addToSelection)
		{
			SelectedVertices.clear();
		}

		SelectedVertices.insert(InIndex);		

		OnSelectionChanged();
	}

	void VertexObject::SelectEdge(int InIndex, bool addToSelection)
	{
		if (!addToSelection)
		{
			SelectedEdges.clear();
		}

		SelectedEdges.insert(InIndex);

		OnSelectionChanged();
	}

	void VertexObject::SelectFace(int InIndex, bool addToSelection)
	{
		if (!addToSelection)
		{
			SelectedFaces.clear();
		}

		SelectedFaces.insert(InIndex);

		OnSelectionChanged();
	}

	void VertexObject::DeselectVertex(int InIndex)
	{
		SelectedVertices.erase(InIndex);
		OnSelectionChanged();
	}

	void VertexObject::DeselectEdge(int InIndex)
	{
		SelectedEdges.erase(InIndex);
		OnSelectionChanged();
	}

	void VertexObject::DeselectFace(int InIndex)
	{
		SelectedFaces.erase(InIndex);
		OnSelectionChanged();
	}

	void VertexObject::ClearSelectedVertices()
	{
		SelectedVertices.clear();
		OnSelectionChanged();
	}

	void VertexObject::ClearSelectedEdges()
	{
		SelectedEdges.clear();
		OnSelectionChanged();
	}

	void VertexObject::ClearSelectedFaces()
	{
		SelectedFaces.clear();
		OnSelectionChanged();
	}

	void VertexObject::OnSelectionChanged()
	{
		// TODO: Optimize by splitting into OnVertexSelectionChanged... for each prim
		// Regenerate vertex arrays for displaying selections

		// Update vertices
		auto vertexPositions = make_unique<FPositionArray>();
		for (size_t i = 0; i < Vertices.size(); i++)
		{
			if (!(SelectedVertices.find(Vertices[i]->GetIndexId()) == SelectedVertices.end()))
			{
				vertexPositions->push_back(Vertices[i]->GetPosition());
			}
		}
		SelectedVertexVertexArray_->UpdatePositions(*std::move(vertexPositions));

		// Update edges
		auto edgePositions = make_unique<FPositionArray>();
		for (size_t i = 0; i < Edges.size(); i++)
		{
			if (!(SelectedEdges.find(Edges[i]->GetIndexId()) == SelectedEdges.end()))
			{
				FEdge* currentEdge = Edges[i].get();
				FVertex* nextVertex = currentEdge->GetFirstHalfEdge()->GetNextVertex();
				FVertex* prevVertex = currentEdge->GetSecondHalfEdge()->GetNextVertex();
				edgePositions->push_back(prevVertex->GetPosition());
				edgePositions->push_back(nextVertex->GetPosition());
			}
		}
		SelectedEdgeVertexArray_->UpdatePositions(*std::move(edgePositions));

		// Update faces
		auto facePositions = make_unique<FPositionArray>();
		auto faceIndices = make_unique<FIndexArray>();

		unsigned int currentVertIndex = 0;

		for (size_t i = 0; i < Faces.size(); i++)
		{
			if (!(SelectedFaces.find(Faces[i]->GetIndexId()) == SelectedFaces.end()))
			{
				// face triangulation
				FFace* currentFace = Faces[i].get();

				FHalfEdge* startingHalfEdge = currentFace->GetHalfEdgeOnFace();
				FVertex* firstVertex = startingHalfEdge->GetNextVertex();
				facePositions->push_back(firstVertex->GetPosition());

				unsigned int firstVertexIndex = currentVertIndex;
				currentVertIndex++;

				FHalfEdge* nextHalfEdge = startingHalfEdge->GetNextHalfEdge();
				std::vector<unsigned int> vertexIndices;
				while (startingHalfEdge != nextHalfEdge)
				{
					FVertex* nextVertex = nextHalfEdge->GetNextVertex();
					facePositions->push_back(nextVertex->GetPosition());
					vertexIndices.push_back(currentVertIndex);
					currentVertIndex++;

					if (vertexIndices.size() == 2)
					{
						// Add triangle with first vertex and 2 vertices found
						faceIndices->push_back(firstVertexIndex);
						faceIndices->push_back(vertexIndices[0]);
						faceIndices->push_back(vertexIndices[1]);

						vertexIndices[0] = vertexIndices[1]; // Shift over vertex
						vertexIndices.pop_back();
					}

					nextHalfEdge = nextHalfEdge->GetNextHalfEdge();
				}
			}
		}
		SelectedFaceVertexArray_->UpdatePositions(*std::move(facePositions));
		SelectedFaceVertexArray_->UpdateIndices(*std::move(faceIndices));

	}

	std::set<FVertex*> VertexObject::GetAggregateSelectedVertices() const
	{
		std::set<FVertex*> aggregateVerts;
		for (FVertex* vertex : GetSelectedVerticesPtrs())
		{
			aggregateVerts.insert(vertex);
		}

		for (FEdge* edge : GetSelectedEdgesPtrs())
		{
			aggregateVerts.insert(edge->GetFirstHalfEdge()->GetNextVertex());
			aggregateVerts.insert(edge->GetSecondHalfEdge()->GetNextVertex());
		}

		for (FFace* face : GetSelectedFacesPtrs())
		{
			for (FVertex* vertex : face->GetVerticesOnFace())
			{
				aggregateVerts.insert(vertex);
			}
		}
		return aggregateVerts;
	}

	glm::vec3 VertexObject::GetSelectedPrimAveragePosition()
	{
		glm::vec3 average = glm::vec3(0.0f);
		std::set<FVertex*> aggregateVerts = GetAggregateSelectedVertices();
		if (aggregateVerts.size() > 0)
		{
			for (FVertex* vert : aggregateVerts)
			{
				average += vert->GetPosition();
			}
			average /= aggregateVerts.size();
		}
		return average;
	}

	void VertexObject::CleanupDeletedPrimitives()
	{
		std::vector<std::unique_ptr<FVertex>> RemainingVertices;
		std::vector<std::unique_ptr<FHalfEdge>> RemainingHalfEdges;
		std::vector<std::unique_ptr<FEdge>> RemainingEdges;
		std::vector<std::unique_ptr<FFace>> RemainingFaces;

		for (int i = 0; i < Vertices.size(); i++)
		{
			if (!Vertices[i]->bMarkedForDeletion)
			{
				FHalfEdge* previousParent = Vertices[i]->GetParentHalfEdge();
				if (previousParent->bMarkedForDeletion)
				{
					std::cerr << "Found non-deleted vertex with parent half edge that is marked for deletion" << std::endl;
				}

				RemainingVertices.push_back(std::move(Vertices[i]));
			}
		}

		for (int i = 0; i < HalfEdges.size(); i++)
		{
			if (!HalfEdges[i]->bMarkedForDeletion)
			{
				RemainingHalfEdges.push_back(std::move(HalfEdges[i]));
			}
			else
			{
				// Previous half edge needs to get a new next
				FHalfEdge* prevHalfEdge = HalfEdges[i]->GetPreviousHalfEdge();
				if (!prevHalfEdge->bMarkedForDeletion)
					prevHalfEdge->SetNextHalfEdge(HalfEdges[i]->GetSymmetricalHalfEdge()->GetNextHalfEdge());
			}
		}

		for (int i = 0; i < Edges.size(); i++)
		{
			if (!Edges[i]->bMarkedForDeletion)
			{
				RemainingEdges.push_back(std::move(Edges[i]));
			}
		}

		for (int i = 0; i < Faces.size(); i++)
		{
			if (!Faces[i]->bMarkedForDeletion)
			{
				RemainingFaces.push_back(std::move(Faces[i]));
			}
		}

		Vertices = std::move(RemainingVertices);
		HalfEdges = std::move(RemainingHalfEdges);
		Edges = std::move(RemainingEdges);
		Faces = std::move(RemainingFaces);

		SelectedVertices.clear();
		SelectedEdges.clear();
		SelectedHalfEdges.clear();
		SelectedFaces.clear();

		MarkDirty();
	}

	void VertexObject::DeleteSelectedVertices()
	{
		std::set<FVertex*> verticesToDelete = GetAggregateSelectedVertices();
		for (FVertex* vertex : verticesToDelete)
		{
			DeleteVertex(vertex);
		}

		CleanupDeletedPrimitives();
	}

	void VertexObject::DeleteVertex(FVertex* InVertex)
	{
		if (InVertex->bMarkedForDeletion) return;
		InVertex->bMarkedForDeletion = true;

		// Delete all incident edges. Start at parent half edge, jump to other side then to next, then to other side then next etc.
		FHalfEdge* startingHalfEdge = InVertex->GetParentHalfEdge();
		FHalfEdge* nextHalfEdge = startingHalfEdge;
		do
		{
			FEdge* owningEdge = nextHalfEdge->GetOwningEdge();
			DeleteEdge(owningEdge);
			nextHalfEdge = nextHalfEdge->GetNextHalfEdge()->GetSymmetricalHalfEdge();
		} while (nextHalfEdge != startingHalfEdge);
	}

	void VertexObject::DeleteHalfEdge(FHalfEdge* InHalfEdge)
	{
		if (InHalfEdge->bMarkedForDeletion) return;

		InHalfEdge->bMarkedForDeletion = true;

		FFace* owningFace = InHalfEdge->GetOwningFace();
		if (owningFace && !owningFace->bMarkedForDeletion)
		{
			DeleteFace(owningFace);
		}

		// Reassign next vertex parent half edge if it references this half edge currently
		FVertex* nextVertex = InHalfEdge->GetNextVertex();
		FHalfEdge* currentParentHalfEdge = nextVertex->GetParentHalfEdge();
		bool bHasOtherIncidentEdge = false;
		if (currentParentHalfEdge == InHalfEdge)
		{
			FHalfEdge* nextHalfEdge = currentParentHalfEdge->GetNextHalfEdge()->GetSymmetricalHalfEdge();
			while (nextHalfEdge != currentParentHalfEdge)
			{
				if (!nextHalfEdge->bMarkedForDeletion)
				{
					bHasOtherIncidentEdge = true;
					nextVertex->SetParentHalfEdge(nextHalfEdge);
					break;
				}
				nextHalfEdge = nextHalfEdge->GetNextHalfEdge()->GetSymmetricalHalfEdge();
			}

			// If there is no other incident edge, delete the vertex
			if (!bHasOtherIncidentEdge)
			{
				DeleteVertex(nextVertex);
			}
		}

	}

	void VertexObject::DeleteEdge(FEdge* InEdge)
	{
		if (InEdge->bMarkedForDeletion) return;

		InEdge->bMarkedForDeletion = true;

		DeleteHalfEdge(InEdge->GetFirstHalfEdge());
		DeleteHalfEdge(InEdge->GetSecondHalfEdge());
	}

	void VertexObject::DeleteFace(FFace* InFace)
	{
		if (InFace->bMarkedForDeletion) return;

		InFace->bMarkedForDeletion = true;

		FHalfEdge* startingHalfEdge = InFace->GetHalfEdgeOnFace();
		FHalfEdge* nextHalfEdge = startingHalfEdge;
		do
		{
			if (nextHalfEdge->GetSymmetricalHalfEdge()->GetOwningFace() == nullptr)
			{
				// If there is no face on the other side of this half edge, we should mark that edge for deletion
				DeleteEdge(nextHalfEdge->GetOwningEdge());
			}
			else
			{
				// If there is a face on the other side, we just null out this half edge
				nextHalfEdge->SetOwningFace(nullptr);
			}
			nextHalfEdge = nextHalfEdge->GetNextHalfEdge();
		} while (startingHalfEdge != nextHalfEdge);
		
	}

	void VertexObject::Render()
	{
		if (HasIndices())
		{
			GetVertexArray().Render(0, GetIndices().size());
		}
		else
		{
			GetVertexArray().Render(0, GetPositions().size());
		}
	}

	void VertexObject::SetDebugNormals(bool InDebugNormals)
	{
		bDebugNormals = InDebugNormals;

		float lineLength = .5f;

		auto vertexPositions = make_unique<FPositionArray>();
		std::vector<glm::vec3>& positionsRef = *Positions;
		std::vector<glm::vec3>& normalsRef = *Normals;

		for (int i = 0; i < positionsRef.size(); i++)
		{
			vertexPositions->push_back(positionsRef[i]);
			vertexPositions->push_back(positionsRef[i] + normalsRef[i] * lineLength);
		}

		NormalsDebugVertexArray_->UpdatePositions(*std::move(vertexPositions));
	}

	void VertexObject::ExtrudeSelectedFaces(EFaceExtrudeType InType)
	{
		
		
		if (InType == EFaceExtrudeType::Individual)
		{
			for (FFace* face : GetSelectedFacesPtrs())
			{
				std::unique_ptr<FFaceRegion> region = make_unique<FFaceRegion>();
				region->FacesInRegion.emplace_back(face);
				std::shared_ptr<FBoundaryLoop> loop = std::make_shared<FBoundaryLoop>();
				loop->Loop = face->GetHalfEdgesOnFace();
				region->BoundaryLoops.emplace_back(std::move(loop));
				ExtrudeRegion(region.get());
			}
		}
		else if (InType == EFaceExtrudeType::Regions)
		{
			// Find all regions
			std::vector<std::unique_ptr<FFaceRegion>> regions = FindRegionsFromFaces(GetSelectedFacesPtrs());
			for (int i = 0; i < regions.size(); i++)
			{
				std::cout << fmt::format("Region {} ", i) << regions[i]->ToString().c_str() << std::endl;
				ExtrudeRegion(regions[i].get());
			}
		}
		
		MarkDirty();
	}

	void VertexObject::ExtrudeRegion(FFaceRegion* InRegion)
	{
		glm::vec3 naiveFaceNormal = InRegion->GetAverageNaiveNormal();
		// For each region, create new edges along the loops, then move the remaining vertices along the averaged normal
		std::set<int> verticesMoved;
		for (size_t loopIndex = 0; loopIndex < InRegion->BoundaryLoops.size(); loopIndex++)
		{
			std::vector<FHalfEdge*> existingLoop;
			std::vector<FHalfEdge*> newHalfEdges; // Holds the newly created edge ring
			for (size_t i = 0; i < InRegion->BoundaryLoops[loopIndex]->Loop.size(); i++)
			{
				FHalfEdge* halfEdge = InRegion->BoundaryLoops[loopIndex]->Loop[i];

				existingLoop.emplace_back(halfEdge);
				FHalfEdge* newHalfEdge = CreateHalfEdge(nullptr, nullptr, nullptr, nullptr, nullptr);

				FVertex* newVertex = CreateVertex(halfEdge->GetNextVertex()->GetPosition() + naiveFaceNormal, halfEdge);
				verticesMoved.insert(newVertex->GetIndexId());

				newHalfEdge->SetNextVertex(newVertex);
				halfEdge->SetNextVertex(newVertex);

				if (i < InRegion->BoundaryLoops[loopIndex]->IncidentHalfEdgesPerLoopEdge.size())
				{
					for (FHalfEdge* incidentHalfEdge : InRegion->BoundaryLoops[loopIndex]->IncidentHalfEdgesPerLoopEdge[i].IncidentHalfEdges)
					{
						incidentHalfEdge->SetNextVertex(newVertex);
					}
				}

				newHalfEdges.emplace_back(newHalfEdge);
			}

			// Using the existing loop and the new half edges (one per edge on the boundary edge ring), create the new faces
			for (size_t i = 0; i < existingLoop.size(); i++)
			{
				FFace* newFace = CreateFace(nullptr);
				newHalfEdges[i]->SetOwningFace(newFace);
				newFace->SetHalfEdgeOnFace(newHalfEdges[i]);

				// Create a new edge for the existing loop half edge, and give it a new symmetric half edge
				FHalfEdge* newSymmetricHalfEdge = CreateHalfEdge(nullptr, existingLoop[i], newFace, nullptr, existingLoop[i]->GetPreviousHalfEdge()->GetNextVertex());
				FEdge* newEdge = CreateEdge(existingLoop[i], newSymmetricHalfEdge);

				// Store important info from existing loop before reassigning symmetrical pointer
				FHalfEdge* previousSymmetricalHalfEdge = existingLoop[i]->GetSymmetricalHalfEdge();
				existingLoop[i]->SetSymmetricalHalfEdge(newSymmetricHalfEdge);

				// Setup previously created half edge
				newHalfEdges[i]->SetNextHalfEdge(newSymmetricHalfEdge);

				// Create half edge on opposite side of face as the previously created half edge
				FHalfEdge* oppositeSideHalfEdge = CreateHalfEdge(nullptr, newHalfEdges[(i + newHalfEdges.size() - 1) % newHalfEdges.size()], newFace, nullptr, previousSymmetricalHalfEdge->GetNextVertex());
				newHalfEdges[(i + newHalfEdges.size() - 1) % newHalfEdges.size()]->SetSymmetricalHalfEdge(oppositeSideHalfEdge);
				newSymmetricHalfEdge->SetNextHalfEdge(oppositeSideHalfEdge);

				// Create edge for this edge of the edge ring
				FEdge* newEdgeRingEdge = CreateEdge(oppositeSideHalfEdge, oppositeSideHalfEdge->GetSymmetricalHalfEdge());

				// Create final half edge for this face
				FHalfEdge* symmetricalHalfEdgeToPreviouslyConnectedFace = CreateHalfEdge(newHalfEdges[i], previousSymmetricalHalfEdge, newFace, previousSymmetricalHalfEdge->GetOwningEdge(), previousSymmetricalHalfEdge->GetPreviousHalfEdge()->GetNextVertex());
				oppositeSideHalfEdge->SetNextHalfEdge(symmetricalHalfEdgeToPreviouslyConnectedFace);
				previousSymmetricalHalfEdge->SetSymmetricalHalfEdge(symmetricalHalfEdgeToPreviouslyConnectedFace);
				previousSymmetricalHalfEdge->GetOwningEdge()->SetFirstHalfEdge(previousSymmetricalHalfEdge);
				previousSymmetricalHalfEdge->GetOwningEdge()->SetSecondHalfEdge(symmetricalHalfEdgeToPreviouslyConnectedFace);
				previousSymmetricalHalfEdge->GetNextVertex()->SetParentHalfEdge(previousSymmetricalHalfEdge);
			}
		}

		for (FFace* face : InRegion->FacesInRegion)
		{
			for (FHalfEdge* halfEdge : face->GetHalfEdgesOnFace())
			{
				FVertex* currentVertex = halfEdge->GetNextVertex();
				if ((verticesMoved.find(currentVertex->GetIndexId()) == verticesMoved.end()))
				{
					verticesMoved.insert(currentVertex->GetIndexId());
					currentVertex->SetPosition(currentVertex->GetPosition() + naiveFaceNormal);
				}
			}
		}
	}


	FVertex* VertexObject::CreateVertex(glm::vec3 InPosition, FHalfEdge* InParentHalfEdge)
	{
		Vertices.emplace_back(std::move(make_unique<FVertex>(InPosition, InParentHalfEdge, VertexIndex)));
		VertexIndex++;
		return Vertices[Vertices.size() - 1].get();
	}

	FHalfEdge* VertexObject::CreateHalfEdge(FHalfEdge* InNextHalfEdge, FHalfEdge* InSymmetricalHalfEdge, FFace* InOwningFace, FEdge* InOwningEdge, FVertex* InNextVertex)
	{
		HalfEdges.emplace_back(std::move(make_unique<FHalfEdge>(InNextHalfEdge, InSymmetricalHalfEdge, InOwningFace, InOwningEdge, InNextVertex, HalfEdgeIndex)));
		HalfEdgeIndex++;
		return HalfEdges[HalfEdges.size() - 1].get();
	}

	FEdge* VertexObject::CreateEdge(FHalfEdge* InFirstHalfEdge, FHalfEdge* InSecondHalfEdge)
	{
		Edges.emplace_back(std::move(make_unique<FEdge>(InFirstHalfEdge, InSecondHalfEdge, EdgeIndex)));
		EdgeIndex++;
		return Edges[Edges.size() - 1].get();
	}

	FFace* VertexObject::CreateFace(FHalfEdge* InHalfEdgeOnFace)
	{
		Faces.emplace_back(std::move(make_unique<FFace>(InHalfEdgeOnFace, FaceIndex)));
		FaceIndex++;
		return Faces[Faces.size() - 1].get();
	}

	bool VertexObject::HandleClick(FRay InSceneRay, Application* InOriginatingApplication)
	{
		// Mouse Ray comes in pre-transformed to vertex object local space
		EEditModeSelectionType selectionType = InOriginatingApplication->GetEditModeSelectionType();
		bool bIsMultiSelect = ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_CONTROL);

		if (selectionType == EEditModeSelectionType::Vertex)
		{
			FVertex* vertexHit = GetVertexUnderRay(InSceneRay);
			if (vertexHit)
			{
				if (bIsMultiSelect)
				{
					if (SelectedVertices.find(vertexHit->GetIndexId()) != SelectedVertices.end())
					{
						DeselectVertex(vertexHit->GetIndexId());
					}
					else
					{
						SelectVertex(vertexHit->GetIndexId(), true);
					}
				}
				else
				{
					SelectVertex(vertexHit->GetIndexId(), false);
				}
			}
			else if (!bIsMultiSelect)
			{
				ClearSelectedVertices();
			}
		}
		else if (selectionType == EEditModeSelectionType::Edge)
		{
			FEdge* edgeHit = GetEdgeUnderRay(InSceneRay);
			if (edgeHit)
			{
				if (bIsMultiSelect)
				{
					if (SelectedEdges.find(edgeHit->GetIndexId()) != SelectedEdges.end())
					{
						DeselectEdge(edgeHit->GetIndexId());
					}
					else
					{
						SelectEdge(edgeHit->GetIndexId(), true);
					}
				}
				else
				{
					SelectEdge(edgeHit->GetIndexId(), false);
				}
			}
			else if (!bIsMultiSelect)
			{
				ClearSelectedEdges();
			}
		}
		else if (selectionType == EEditModeSelectionType::Face)
		{
			FFace* faceHit = GetFaceUnderRay(InSceneRay);
			if (faceHit)
			{
				if (bIsMultiSelect)
				{
					if (SelectedFaces.find(faceHit->GetIndexId()) != SelectedFaces.end())
					{
						DeselectFace(faceHit->GetIndexId());
					}
					else
					{
						SelectFace(faceHit->GetIndexId(), true);
					}
				}
				else
				{
					SelectFace(faceHit->GetIndexId(), false);
				}
			}
			else if (!bIsMultiSelect)
			{
				ClearSelectedFaces();
			}
		}

		return false;
	}

	FVertex* VertexObject::GetVertexUnderRay(FRay InSceneRay)
	{
		FHitRecord hitRecord = FHitRecord();
		bool bWasAnyVertexFound = false;
		FVertex* hitVertex = nullptr;
		for (size_t i = 0; i < Vertices.size(); i++)
		{
			SphereHittable vertexCollision = SphereHittable(.15f, Vertices[i]->GetPosition());
			bool hitRecorded = vertexCollision.Intersect(InSceneRay, 0.0001f, hitRecord);
			bWasAnyVertexFound |= hitRecorded;
			if (hitRecorded) hitVertex = Vertices[i].get();
		}

		if (bWasAnyVertexFound)
		{
			return hitVertex;
		}
		else 
		{
			return nullptr;
		}
	}

	FEdge* VertexObject::GetEdgeUnderRay(FRay InSceneRay)
	{
		FHitRecord hitRecord = FHitRecord();
		bool bWasAnyEdgeFound = false;
		FEdge* hitEdge = nullptr;
		for (size_t i = 0; i < Edges.size(); i++)
		{
			glm::vec3 firstVertexPos = Edges[i]->GetFirstHalfEdge()->GetNextVertex()->GetPosition();
			glm::vec3 secondVertexPos = Edges[i]->GetSecondHalfEdge()->GetNextVertex()->GetPosition();

			CylinderHittable edgeCollision = CylinderHittable(.15f, firstVertexPos, glm::normalize(secondVertexPos - firstVertexPos), glm::length(secondVertexPos - firstVertexPos));
			bool hitRecorded = edgeCollision.Intersect(InSceneRay, 0.0001f, hitRecord);
			bWasAnyEdgeFound |= hitRecorded;
			if (hitRecorded) hitEdge = Edges[i].get();
		}

		if (bWasAnyEdgeFound)
		{
			return hitEdge;
		}
		else
		{
			return nullptr;
		}
	}

	FFace* VertexObject::GetFaceUnderRay(FRay InSceneRay)
	{
		FHitRecord hitRecord = FHitRecord();
		bool bWasAnyFaceFound = false;
		FFace* hitFace = nullptr;
		for (size_t i = 0; i < Faces.size(); i++)
		{
			for (TriangleHittable triangle : Faces[i]->GetTrianglesOnFace())
			{
				bool hitRecorded = triangle.Intersect(InSceneRay, 0.0001f, hitRecord);
				bWasAnyFaceFound |= hitRecorded;
				if (hitRecorded) hitFace = Faces[i].get();
			}
		}

		if (bWasAnyFaceFound)
		{
			return hitFace;
		}
		else
		{
			return nullptr;
		}
	}

}


