#include "ScrewModifier.h"
#include <unordered_map>

namespace CHISTUDIO {

void ScrewModifier::ApplyModifier(VertexObject* InObjectToModify) const
{
	// Get boundary loops
	std::vector<FFace*> allFaces;
	auto& faces = InObjectToModify->GetFaces();
	for (size_t i = 0; i < faces.size(); i++)
	{
		allFaces.emplace_back(faces[i].get());
	}
	std::vector<std::unique_ptr<FFaceRegion>> regions = InObjectToModify->FindRegionsFromFaces(allFaces);

	std::vector<std::shared_ptr<FBoundaryLoop>> boundaryLoops;
	std::vector<std::shared_ptr<FBoundaryLoop>> startingBoundaryLoops;
	for (size_t i = 0; i < regions.size(); i++)
	{
		for (size_t loopIndex = 0; loopIndex < regions[i]->BoundaryLoops.size(); loopIndex++)
		{
			// Flip to outer loop
			std::shared_ptr<FBoundaryLoop> flippedBoundaryLoop = std::make_shared<FBoundaryLoop>();
			std::shared_ptr<FBoundaryLoop> unflippedBoundaryLoop = std::make_shared<FBoundaryLoop>();
			FHalfEdge* startingHalfEdge = regions[i]->BoundaryLoops[loopIndex]->Loop[0]->GetSymmetricalHalfEdge();
			FHalfEdge* currentHalfEdge = startingHalfEdge;
			FHalfEdge* currentUnFlippedHalfEdge = startingHalfEdge->GetSymmetricalHalfEdge();
			do
			{
				flippedBoundaryLoop->Loop.emplace_back(currentHalfEdge);
				currentHalfEdge = currentHalfEdge->GetNextHalfEdge();

				unflippedBoundaryLoop->Loop.emplace_back(currentUnFlippedHalfEdge);
				currentUnFlippedHalfEdge = currentUnFlippedHalfEdge->GetSymmetricalHalfEdge()->GetPreviousHalfEdge()->GetSymmetricalHalfEdge();
			} while (startingHalfEdge != currentHalfEdge);
			boundaryLoops.emplace_back(flippedBoundaryLoop);
			startingBoundaryLoops.emplace_back(unflippedBoundaryLoop);
		}
	}

	// Calculate transform matrix
	glm::mat4 transformMatrix(1.f);

	glm::vec3 axis;
	switch (ScrewAxis)
	{
	case(EScrewAxis::X):
		axis = glm::vec3(1.0f, 0.0f, 0.0f);
		break;
	case(EScrewAxis::Y):
		axis = glm::vec3(0.0f, 1.0f, 0.0f);
		break;
	case(EScrewAxis::Z):
		axis = glm::vec3(0.0f, 0.0f, 1.0f);
		break;
	}

	glm::vec3 rotationPerStep = axis * (-AngleInDegrees / (float)Steps);
	glm::vec3 translationPerStep = axis * (Height / (float)Steps);
	// Order: scale, rotate, translate
	transformMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.0f)) * transformMatrix;
	transformMatrix = glm::mat4_cast(glm::quat(glm::radians(rotationPerStep))) * transformMatrix;
	transformMatrix = glm::translate(glm::mat4(1.f), translationPerStep) * transformMatrix;

	// Iterate over boundary loops and extrude
	std::vector<std::shared_ptr<FBoundaryLoop>> nextBoundaryLoops;
	int iterations = MergeEndWithStart ? Steps - 1 : Steps;
	for (int stepIndex = 0; stepIndex < iterations; stepIndex++)
	{
		for (size_t i = 0; i < boundaryLoops.size(); i++)
		{
			std::shared_ptr<FBoundaryLoop> newLoop = ExtrudeBoundaryLoop(InObjectToModify, boundaryLoops[i].get(), transformMatrix);
			nextBoundaryLoops.emplace_back(std::move(newLoop));
		}
		boundaryLoops.swap(nextBoundaryLoops);
		nextBoundaryLoops.clear();
	}

	if (MergeEndWithStart)
	{
		InObjectToModify->DeleteFaces(allFaces);
		// Bridge each loop to starting loop
		for (size_t i = 0; i < boundaryLoops.size(); i++)
		{
			InObjectToModify->BridgeBoundaryLoops(startingBoundaryLoops[i].get(), boundaryLoops[i].get());
		}
	}
}

bool ScrewModifier::RenderUI()
{
	bool wasModified = false;
	wasModified |= ImGui::DragInt("Steps", &Steps, 1.0f, 1, 200);
	wasModified |= ImGui::DragFloat("Angle (Degrees)", &AngleInDegrees, 1.0f);
	wasModified |= ImGui::DragFloat("Height", &Height, 1.0f);
	int axis = (int)ScrewAxis;
	wasModified |= ImGui::RadioButton("X", &axis, 0); ImGui::SameLine();
	wasModified |= ImGui::RadioButton("Y", &axis, 1); ImGui::SameLine();
	wasModified |= ImGui::RadioButton("Z", &axis, 2);
	wasModified |= ImGui::Checkbox("Close Loop", &MergeEndWithStart);
	ScrewAxis = (EScrewAxis)axis;

	return wasModified;
}

std::shared_ptr<FBoundaryLoop> ScrewModifier::ExtrudeBoundaryLoop(VertexObject* InObjectToModify, FBoundaryLoop* InBoundaryLoop, glm::mat4 InTransformMatrix) const
{
	std::shared_ptr<FBoundaryLoop> newBoundaryLoop = std::make_shared<FBoundaryLoop>();

	std::vector<FVertex*> newVertices;
	std::vector<FHalfEdge*> newHalfEdgeRing;
	for (FHalfEdge* halfEdge : InBoundaryLoop->Loop)
	{
		// For each half edge, we create 1 vert, 1 face, and 4 half edges (three on the face, one on the new boundary)
		glm::vec3 newPos = InTransformMatrix * glm::vec4(halfEdge->GetNextVertex()->GetPosition(), 1.0f);
		//std::cout << "Original: " << glm::to_string(halfEdge->GetNextVertex()->GetPosition()) << std::endl;
		//std::cout << "New: " << glm::to_string(newPos) << std::endl;
		FVertex* newVert = InObjectToModify->CreateVertex(newPos, nullptr);
		newVertices.emplace_back(newVert);

		FFace* newFace = InObjectToModify->CreateFace(halfEdge);

		FHalfEdge* nextNewHalfEdge = InObjectToModify->CreateHalfEdge(nullptr, nullptr, newFace, nullptr, newVert); // After this loop, Need to set Symmetrical and Edge
		halfEdge->SetNextHalfEdge(nextNewHalfEdge);
		halfEdge->SetOwningFace(newFace);
		newVert->SetParentHalfEdge(nextNewHalfEdge);
		FHalfEdge* previousNewHalfEdge = InObjectToModify->CreateHalfEdge(halfEdge, nullptr, newFace, nullptr, halfEdge->GetPreviousHalfEdge()->GetNextVertex()); // After this loop, Need to set Symmetrical and Edge
		FHalfEdge* topNewHalfEdge = InObjectToModify->CreateHalfEdge(previousNewHalfEdge, nullptr, newFace, nullptr, nullptr); // After this loop, Need to set NextVertex
		FHalfEdge* boundaryNewHalfEdge = InObjectToModify->CreateHalfEdge(nullptr, topNewHalfEdge, nullptr, nullptr, newVert); // After this loop, Need to set NextHalfEdge
		topNewHalfEdge->SetSymmetricalHalfEdge(boundaryNewHalfEdge);
		nextNewHalfEdge->SetNextHalfEdge(topNewHalfEdge);
		FEdge* newEdge = InObjectToModify->CreateEdge(topNewHalfEdge, boundaryNewHalfEdge);

		newHalfEdgeRing.emplace_back(previousNewHalfEdge);
		newHalfEdgeRing.emplace_back(nextNewHalfEdge);

		newBoundaryLoop->Loop.emplace_back(boundaryNewHalfEdge);
	}

	size_t loopSize = newBoundaryLoop->Loop.size();
	for (size_t i = 0; i < loopSize; i++)
	{
		newBoundaryLoop->Loop[i]->SetNextHalfEdge(newBoundaryLoop->Loop[(i + 1) % loopSize]);
	}
	for (size_t i = 0; i < loopSize; i++)
	{
		FVertex* nextVert = newBoundaryLoop->Loop[i]->GetPreviousHalfEdge()->GetNextVertex();
		newBoundaryLoop->Loop[i]->GetSymmetricalHalfEdge()->SetNextVertex(nextVert);

		FHalfEdge* firstHalfEdge = newHalfEdgeRing[i * 2];
		FHalfEdge* secondHalfEdge = newHalfEdgeRing[((i * 2) + (loopSize * 2) - 1) % (loopSize * 2)];
		firstHalfEdge->SetSymmetricalHalfEdge(secondHalfEdge);
		secondHalfEdge->SetSymmetricalHalfEdge(firstHalfEdge);
		FEdge* newEdge = InObjectToModify->CreateEdge(firstHalfEdge, secondHalfEdge);
	}

	return newBoundaryLoop;
}


}
