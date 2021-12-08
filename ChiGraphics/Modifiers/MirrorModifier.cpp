#include "MirrorModifier.h"
#include "ChiGraphics/Meshes/VertexObject.h"
#include <unordered_map>

namespace CHISTUDIO {

void MirrorModifier::ApplyModifier(VertexObject* InObjectToModify) const
{
	// Apply hard-coded reflection matrices. TODO: Reflection across an arbitrary plane https://gamedev.stackexchange.com/questions/43615/how-can-i-reflect-a-point-with-respect-to-the-plane/43692#43692
	if (MirrorX)
	{
		// YZ plane
		glm::mat4 reflectionMatrix = glm::mat4
			(-1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f);

		ApplyMirrorFromReflectionMatrix(InObjectToModify, reflectionMatrix);
	}
	if (MirrorY)
	{ 
		// XZ plane
		glm::mat4 reflectionMatrix = glm::mat4
		(1.f, 0.f, 0.f, 0.f,
			0.f, -1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f);

		ApplyMirrorFromReflectionMatrix(InObjectToModify, reflectionMatrix);
	}
	if (MirrorZ)
	{
		// XY plane
		glm::mat4 reflectionMatrix = glm::mat4
		(1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, -1.f, 0.f,
			0.f, 0.f, 0.f, 1.f);

		ApplyMirrorFromReflectionMatrix(InObjectToModify, reflectionMatrix);
	}
}

bool MirrorModifier::RenderUI()
{
	bool wasModified = false;
	wasModified |= ImGui::Checkbox("X", &MirrorX);
	ImGui::SameLine();	  
	wasModified |= ImGui::Checkbox("Y", &MirrorY);
	ImGui::SameLine();	  
	wasModified |= ImGui::Checkbox("Z", &MirrorZ);

	return wasModified;
}

void MirrorModifier::ApplyMirrorFromReflectionMatrix(VertexObject* InObjectToModify, glm::mat4 InReflectionMatrix) const
{
	// Copy all prims, transform vertices, and reverse half edge directions
	auto& vertices = InObjectToModify->GetVertices();
	auto& faces = InObjectToModify->GetFaces();
	auto& halfEdges = InObjectToModify->GetHalfEdges();
	auto& edges = InObjectToModify->GetEdges();
	size_t numberOfOriginalVertices = vertices.size();
	size_t numberOfOriginalFaces = faces.size();
	size_t numberOfOriginalHalfEdges = halfEdges.size();
	size_t numberOfOriginalEdges = edges.size();

	std::unordered_map<FVertex*, FVertex*> originalVertexToNewVertex;
	for (size_t i = 0; i < numberOfOriginalVertices; i++)
	{
		FVertex* originalVert = vertices[i].get();
		glm::vec3 reflectedPos = InReflectionMatrix * glm::vec4(originalVert->GetPosition(), 1.0f);
		FVertex* newVert = InObjectToModify->CreateVertex(reflectedPos, nullptr);
		originalVertexToNewVertex.insert({originalVert, newVert });
	}

	std::unordered_map<FHalfEdge*, FHalfEdge*> originalHalfEdgeToNewHalfEdge;
	for (size_t i = 0; i < numberOfOriginalHalfEdges; i++)
	{
		FHalfEdge* originalHalfEdge = halfEdges[i].get();
		FVertex* nextVertex = originalVertexToNewVertex.find(originalHalfEdge->GetPreviousHalfEdge()->GetNextVertex())->second; // Get the next of previous in order to reverse direction
		FHalfEdge* newHalfEdge = InObjectToModify->CreateHalfEdge(nullptr, nullptr, nullptr, nullptr, nextVertex);
		originalHalfEdgeToNewHalfEdge.insert({ originalHalfEdge, newHalfEdge });
	}

	std::unordered_map<FFace*, FFace*> originalFaceToNewFace;
	for (size_t i = 0; i < numberOfOriginalFaces; i++)
	{
		FFace* originalFace = faces[i].get();
		FHalfEdge* halfEdgeOnFace = originalHalfEdgeToNewHalfEdge.find(originalFace->GetHalfEdgeOnFace())->second;
		FFace* newFace = InObjectToModify->CreateFace(halfEdgeOnFace);
		originalFaceToNewFace.insert({ originalFace, newFace });
	}

	// Finish linking
	for (auto vertexPair : originalVertexToNewVertex)
	{
		FVertex* originalVert = vertexPair.first;
		FVertex* newVert = vertexPair.second;
		FHalfEdge* parentHalfEdge = originalHalfEdgeToNewHalfEdge.find(originalVert->GetParentHalfEdge()->GetNextHalfEdge())->second; // Get parent's next in order to reverse direction
		newVert->SetParentHalfEdge(parentHalfEdge);
	}
	
	for (auto halfEdgePair : originalHalfEdgeToNewHalfEdge)
	{
		FHalfEdge* originalHalfEdge = halfEdgePair.first;
		FHalfEdge* newHalfEdge = halfEdgePair.second;

		FHalfEdge* nextHalfEdge = originalHalfEdgeToNewHalfEdge.find(originalHalfEdge->GetPreviousHalfEdge())->second; // Get previous instead of next in order to reverse direction
		FHalfEdge* symmHalfEdge = originalHalfEdgeToNewHalfEdge.find(originalHalfEdge->GetSymmetricalHalfEdge())->second;
		FFace* owningFace = originalFaceToNewFace.find(originalHalfEdge->GetOwningFace())->second;
		newHalfEdge->SetNextHalfEdge(nextHalfEdge);
		newHalfEdge->SetSymmetricalHalfEdge(symmHalfEdge);
		newHalfEdge->SetOwningFace(owningFace);
	}

	for (size_t i = 0; i < numberOfOriginalEdges; i++)
	{
		FEdge* originalEdge = edges[i].get();
		FHalfEdge* firstHalfEdge = originalHalfEdgeToNewHalfEdge.find(originalEdge->GetFirstHalfEdge())->second;
		FHalfEdge* secondHalfEdge = originalHalfEdgeToNewHalfEdge.find(originalEdge->GetSecondHalfEdge())->second;

		FEdge* newEdge = InObjectToModify->CreateEdge(firstHalfEdge, secondHalfEdge);
	}
}

}
