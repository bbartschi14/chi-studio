#include "D:\GraphicsProjects\ChiStudio\ChiCore\UI\WEditMode.h"
#include "ChiGraphics/Application.h"
#include "UILibrary.h"
#include <glm/gtc/type_ptr.hpp>
#include "core.h"
#include <glm/gtx/matrix_decompose.hpp>


namespace CHISTUDIO {
	WEditMode::WEditMode()
	{
		PrimNudgeAmount = .25f;
		RotationOperationValue = glm::vec3(0.0f);
		ScaleOperationValue = glm::vec3(1.0f);
	}

	void WEditMode::Render(Application& InApplication, float InDeltaTime)
	{
		std::vector<SceneNode*> selectedNodes = InApplication.GetSelectedNodes();

		if (selectedNodes.size() == 1 && InApplication.GetSceneMode() == ESceneMode::Edit)
		{
			ImGui::Begin("Edit Mode");
			
			const char* typeStrings[] = { "Vertex", "Edge", "Face" };
			const char* currentTypeString = typeStrings[(int)InApplication.GetEditModeSelectionType()];
			if (ImGui::BeginCombo("Selection Mode", currentTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentTypeString == typeStrings[i];
					if (ImGui::Selectable(typeStrings[i], isSelected))
					{
						currentTypeString = typeStrings[i];
						InApplication.SetEditModeSelectionType((EEditModeSelectionType)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}


			if (RenderingComponent* renderingComponent = selectedNodes[0]->GetComponentPtr<RenderingComponent>())
			{
				RenderPrimitivesSection(selectedNodes, renderingComponent, InApplication);
			}

			ImGui::End();

		}
	}

	void WEditMode::RenderPrimitivesSection(std::vector<SceneNode*> selectedNodes, RenderingComponent* renderingComponent, Application& InApplication)
	{
		VertexObject* vertexObject = renderingComponent->GetVertexObjectPtr();
		ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

		if (InApplication.AreEditModeVerticesSelectable())
		{
			if (ImGui::CollapsingHeader("Vertices"))
			{
				if (ImGui::BeginTable("VerticesTable", 1, tableFlags))
				{
					ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_NoHide);
					ImGui::TableHeadersRow();
					ImGui::TableNextColumn();

					const std::vector<std::unique_ptr<FVertex>>& vertices = vertexObject->GetVertices();
					const std::set<int>& selectedVertices = vertexObject->GetSelectedVertices();
					for (int i = 0; i < vertices.size(); i++)
					{
						ImGuiTreeNodeFlags leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;// | ImGuiTreeNodeFlags_FramePadding;
						bool bIsVertexSelected = selectedVertices.find(vertices[i]->GetIndexId()) != selectedVertices.end();
						if (bIsVertexSelected)
						{
							leafFlags |= ImGuiTreeNodeFlags_Selected;
							ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ 0.08f, 0.53f, 0.85f, 1.0f });
						}

						// Headers
						ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.025f, 0.43f, 0.75f, 1.0f });
						bool open = ImGui::TreeNodeEx(fmt::format("{}", vertices[i]->GetIndexId()).c_str(), leafFlags); //
						ImGui::PopStyleColor();

						if (bIsVertexSelected)
						{
							ImGui::PopStyleColor();
						}

						if (ImGui::IsItemClicked())
						{
							if (ImGui::GetIO().KeyCtrl)
							{
								if (bIsVertexSelected)
								{
									vertexObject->DeselectVertex(vertices[i]->GetIndexId());
								}
								else
								{
									vertexObject->SelectVertex(vertices[i]->GetIndexId(), true);
								}
							}
							else
							{
								vertexObject->SelectVertex(vertices[i]->GetIndexId(), false);
							}
						}

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
					}
					ImGui::EndTable();
					ImGui::Separator();
				}
			}
		}

		if (InApplication.AreEditModeEdgesSelectable())
		{
			if (ImGui::CollapsingHeader("Edges"))
			{
				if (ImGui::BeginTable("EdgesTable", 1, tableFlags))
				{
					ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_NoHide);
					ImGui::TableHeadersRow();
					ImGui::TableNextColumn();

					const std::vector<std::unique_ptr<FEdge>>& edges = vertexObject->GetEdges();
					const std::set<int>& selectedEdges = vertexObject->GetSelectedEdges();
					for (int i = 0; i < edges.size(); i++)
					{
						ImGuiTreeNodeFlags leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;
						bool bIsEdgeSelected = selectedEdges.find(edges[i]->GetIndexId()) != selectedEdges.end();;
						if (bIsEdgeSelected)
						{
							leafFlags |= ImGuiTreeNodeFlags_Selected;
							ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ 0.08f, 0.53f, 0.85f, 1.0f });
						}

						// Headers
						ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.025f, 0.43f, 0.75f, 1.0f });

						bool open = ImGui::TreeNodeEx(fmt::format("{}", edges[i]->GetIndexId()).c_str(), leafFlags); //
						ImGui::PopStyleColor();
						if (bIsEdgeSelected)
						{
							ImGui::PopStyleColor();
						}

						if (ImGui::IsItemClicked())
						{
							if (ImGui::GetIO().KeyCtrl)
							{
								if (bIsEdgeSelected)
								{
									vertexObject->DeselectEdge(edges[i]->GetIndexId());
								}
								else
								{
									vertexObject->SelectEdge(edges[i]->GetIndexId(), true);
								}
							}
							else
							{
								vertexObject->SelectEdge(edges[i]->GetIndexId(), false);
							}
						}

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
					}
					ImGui::EndTable();
					ImGui::Separator();
				}
			}
		}

		if (InApplication.AreEditModeFacesSelectable())
		{
			if (ImGui::CollapsingHeader("Faces"))
			{
				if (ImGui::BeginTable("FacesTable", 1, tableFlags))
				{
					ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_NoHide);
					ImGui::TableHeadersRow();
					ImGui::TableNextColumn();

					const std::vector<std::unique_ptr<FFace>>& faces = vertexObject->GetFaces();
					const std::set<int>& selectedFaces = vertexObject->GetSelectedFaces();
					for (int i = 0; i < faces.size(); i++)
					{
						ImGuiTreeNodeFlags leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;
						bool bIsFaceSelected = selectedFaces.find(faces[i]->GetIndexId()) != selectedFaces.end();;
						if (bIsFaceSelected)
						{
							leafFlags |= ImGuiTreeNodeFlags_Selected;
							ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ 0.08f, 0.53f, 0.85f, 1.0f });
						}

						// Headers
						ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.025f, 0.43f, 0.75f, 1.0f });

						bool open = ImGui::TreeNodeEx(fmt::format("{}", faces[i]->GetIndexId()).c_str(), leafFlags); //
						ImGui::PopStyleColor();
						if (bIsFaceSelected)
						{
							ImGui::PopStyleColor();
						}

						if (ImGui::IsItemClicked())
						{
							if (ImGui::GetIO().KeyCtrl)
							{
								if (bIsFaceSelected)
								{
									vertexObject->DeselectFace(faces[i]->GetIndexId());
								}
								else
								{
									vertexObject->SelectFace(faces[i]->GetIndexId(), true);
								}
							}
							else
							{
								vertexObject->SelectFace(faces[i]->GetIndexId(), false);
							}
						}

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
					}
					ImGui::EndTable();
					ImGui::Separator();
				}
			}
		}

		if (vertexObject->GetNumberOfPrimsSelected() > 0)
		{
			if (ImGui::CollapsingHeader("Selection Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				glm::vec3 displayPosition = vertexObject->GetSelectedPrimAveragePosition();
				glm::vec3 initialPosition = displayPosition;
				if (UILibrary::DrawVector3Control("Position", displayPosition, 0.1f))
				{
					vertexObject->MoveSelectedPrims(displayPosition - initialPosition);
				}

				bool wasRotationDeactivated = false;
				glm::vec3 previousRotationValue = RotationOperationValue;
				if (UILibrary::DrawVector3ControlWithStates("Rotation", RotationOperationValue, wasRotationDeactivated, 1.f))
				{
					vertexObject->RotateSelectedPrims(RotationOperationValue - previousRotationValue);
				}
				if (wasRotationDeactivated)
				{
					RotationOperationValue = glm::vec3(0.0f);
				}

				bool wasScaleDeactivated = false;
				if (UILibrary::DrawVector3ControlWithStates("Scale", ScaleOperationValue, wasScaleDeactivated, .01f))
				{
					if (PreScaleVertexPosition.size() == 0)
					{
						// Cache pre scale positions
						std::set<FVertex*> verticesToScale = vertexObject->GetAggregateSelectedVertices();
						for (FVertex* vert : verticesToScale)
						{
							PreScaleVertexPosition.push_back(vert->GetPosition());
						}
						StartingScaleOrigin = vertexObject->GetSelectedPrimAveragePosition();
					}
					vertexObject->ScaleSelectedPrims(ScaleOperationValue, StartingScaleOrigin, PreScaleVertexPosition);
				}
				if (wasScaleDeactivated)
				{
					ScaleOperationValue = glm::vec3(1.0f);
					PreScaleVertexPosition.clear();
				}
				ImGui::Separator();
			}
		}


		if (ImGui::CollapsingHeader("Operations", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("Nudge Amount", &PrimNudgeAmount, 0.05f, 0.0f, 200.0f);
			ImVec2 buttonSize = { 35.0f, 20.0f };

			if (ImGui::Button("X+", buttonSize))
			{
				vertexObject->MoveSelectedPrims(glm::vec3(PrimNudgeAmount, 0.0f, 0.0f));
			} ImGui::SameLine();
			if (ImGui::Button("X-", buttonSize))
			{
				vertexObject->MoveSelectedPrims(glm::vec3(-PrimNudgeAmount, 0.0f, 0.0f));
			} ImGui::SameLine();
			if (ImGui::Button("Y+", buttonSize))
			{
				vertexObject->MoveSelectedPrims(glm::vec3(0.0f, PrimNudgeAmount, 0.0f));
			} ImGui::SameLine();
			if (ImGui::Button("Y-", buttonSize))
			{
				vertexObject->MoveSelectedPrims(glm::vec3(0.0f, -PrimNudgeAmount, 0.0f));
			} ImGui::SameLine();
			if (ImGui::Button("Z+", buttonSize))
			{
				vertexObject->MoveSelectedPrims(glm::vec3(0.0f, 0.0f, PrimNudgeAmount));
			} ImGui::SameLine();
			if (ImGui::Button("Z-", buttonSize))
			{
				vertexObject->MoveSelectedPrims(glm::vec3(0.0f, 0.0f, -PrimNudgeAmount));
			}

			if (ImGui::Button("Delete"))
				ImGui::OpenPopup("deletePopup");

			if (ImGui::BeginPopup("deletePopup"))
			{
				if (ImGui::Selectable("Vertices"))
				{
					//Delete vertices
					vertexObject->DeleteSelectedVertices();
				}
				if (ImGui::Selectable("Edges"))
				{
					//Delete edges
				}
				if (ImGui::Selectable("Faces"))
				{
					//Delete faces
					vertexObject->DeleteFaces(vertexObject->GetSelectedFacesPtrs());
				}
				ImGui::Separator();
				ImGui::Text("Type");

				ImGui::EndPopup();
			}

			if (vertexObject->GetSelectedFaces().size() > 0)
			{
				if (ImGui::Button("Extrude"))
					ImGui::OpenPopup("extrudePopup");

				if (ImGui::BeginPopup("extrudePopup"))
				{
					if (ImGui::Selectable("Individual Faces"))
					{
						vertexObject->ExtrudeSelectedFaces(EFaceExtrudeType::Individual);
					}

					if (ImGui::Selectable("Regions"))
					{
						vertexObject->ExtrudeSelectedFaces(EFaceExtrudeType::Regions);
					}
					
					ImGui::Separator();
					ImGui::Text("Mode");

					ImGui::EndPopup();
				}
			}

			if (vertexObject->GetSelectedEdges().size() > 0)
			{
				if (ImGui::Button("Extrude"))
					ImGui::OpenPopup("extrudeEdgesPopup");

				if (ImGui::BeginPopup("extrudeEdgesPopup"))
				{
					if (ImGui::Selectable("Individual Edges"))
					{
						vertexObject->ExtrudeSelectedEdges(EFaceExtrudeType::Individual, glm::vec3(1.0f));
					}

					ImGui::Separator();
					ImGui::Text("Mode");

					ImGui::EndPopup();
				}
			}
		}
	}

}
