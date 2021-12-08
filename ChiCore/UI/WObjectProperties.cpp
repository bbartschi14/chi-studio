#include "D:\GraphicsProjects\ChiStudio\ChiCore\UI\WObjectProperties.h"
#include "ChiGraphics/Application.h"
#include "UILibrary.h"
#include <glm/gtc/type_ptr.hpp>
#include "core.h"
#include "ChiGraphics/Cameras/TracingCameraNode.h"
#include "ChiGraphics/Lights/HittableLight.h"
#include "ChiGraphics/Modifiers/SubdivisionSurfaceModifier.h"

namespace CHISTUDIO {
	WObjectProperties::WObjectProperties()
	{
	}
	void WObjectProperties::Render(Application& InApplication)
	{
		std::vector<SceneNode*> selectedNodes = InApplication.GetSelectedNodes();

		ImGui::Begin("Properties");


		// Currently only work with single select
		if (selectedNodes.size() == 1)
		{
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, selectedNodes[0]->GetNodeName().c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				selectedNodes[0]->SetNodeName(std::string(buffer));
			}

			ImGui::Separator();
		}
		else if (selectedNodes.size() > 1)
		{
			// Indicate multiple selection
			ImGui::Text("Multiple Nodes Selected");
			ImGui::Separator();
		}

		
		RenderTransformPanel(selectedNodes);
		RenderComponents(selectedNodes, InApplication);

		ImGui::End();

	}

	void WObjectProperties::RenderTransformPanel(std::vector<SceneNode*> selectedNodes)
	{
		ImVec2 panelSize = ImGui::GetContentRegionAvail();

		if (selectedNodes.size() == 1)
		{
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				Transform& transform = selectedNodes[0]->GetTransform();
				glm::vec3 position = transform.GetPosition();

				if (UILibrary::DrawVector3Control("Position", position, 0.1f))
				{
					transform.SetPosition(position);
				}

				glm::vec3 rotationEuler = transform.GetEulerRotation();
				
				if (UILibrary::DrawVector3Control("Rotation", rotationEuler, 0.1f))
				{
					transform.SetRotation(rotationEuler);
				}

				glm::vec3 scale = transform.GetScale();

				if (UILibrary::DrawVector3Control("Scale", scale, 0.1f, 1.0f))
				{
					transform.SetScale(scale);
				}

				ImGui::Separator();
			}
		}
	}

	void WObjectProperties::RenderComponents(std::vector<SceneNode*> selectedNodes, Application& InApplication)
	{
		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		if (selectedNodes.size() == 1)
		{
			if (LightComponent* light = selectedNodes[0]->GetComponentPtr<LightComponent>())
			{
				RenderLightComponent(selectedNodes, light);
			}
			if (CameraComponent* camera = selectedNodes[0]->GetComponentPtr<CameraComponent>())
			{
				RenderCameraComponent(selectedNodes, camera);
			}
			if (RenderingComponent* render = selectedNodes[0]->GetComponentPtr<RenderingComponent>())
			{
				if (!render->bIsDebugRender)
					RenderRenderingComponent(selectedNodes, render, InApplication);
			}
			if (MaterialComponent* material = selectedNodes[0]->GetComponentPtr<MaterialComponent>())
			{
				RenderMaterialComponent(selectedNodes, material);
			}
			
		}
	}

	void WObjectProperties::RenderLightComponent(std::vector<SceneNode*> selectedNodes, LightComponent* lightComponent) 
	{
		ImVec2 panelSize = ImGui::GetContentRegionAvail();

		if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ELightType lightType = lightComponent->GetLightType();

			bool isLightEnabled = lightComponent->GetLightPtr()->IsLightEnabled();
			if (ImGui::Checkbox("Enabled", &isLightEnabled))
			{
				lightComponent->GetLightPtr()->SetLightEnabled(isLightEnabled);
			}

			if (lightType == ELightType::Ambient)
			{
				glm::vec3 diffuseColor = lightComponent->GetLightPtr()->GetDiffuseColor();
				glm::vec3 specularColor = lightComponent->GetLightPtr()->GetSpecularColor();
				ImGui::Text("Diffuse Color");
				ImGui::SetNextItemWidth(panelSize.x);
				if (ImGui::ColorEdit3("##Diffuse Color", glm::value_ptr(diffuseColor)))
				{
					lightComponent->GetLightPtr()->SetDiffuseColor(diffuseColor);
				}
			}
			else if (lightType == ELightType::Point)
			{
				glm::vec3 diffuseColor = lightComponent->GetLightPtr()->GetDiffuseColor();
				glm::vec3 specularColor = lightComponent->GetLightPtr()->GetSpecularColor();
				ImGui::Text("Diffuse Color");
				ImGui::SetNextItemWidth(panelSize.x);
				if (ImGui::ColorEdit3("##Diffuse Color", glm::value_ptr(diffuseColor)))
				{
					lightComponent->GetLightPtr()->SetDiffuseColor(diffuseColor);
				}
			}
			else if (lightType == ELightType::Directional)
			{

			}
			else if (lightType == ELightType::Hittable)
			{
				
			}
			ImGui::Separator();

		}
	}

	void WObjectProperties::RenderRenderingComponent(std::vector<SceneNode*> selectedNodes, RenderingComponent* renderingComponent, Application& InApplication)
	{
		if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
		{
			VertexObject* vertexObject = renderingComponent->GetVertexObjectPtr();

			if (ImGui::Button("Copy Object"))
			{
				InApplication.CreateVertexObjectCopy(vertexObject);
			}
			//ImGui::Text("Vertex Object Data");
			//ImGui::BulletText(fmt::format("Vertices: {}", vertexObject->HasPositions() ? vertexObject->GetPositions().size() : 0).c_str());
			//ImGui::BulletText(fmt::format("Tris: {}", vertexObject->HasIndices() ? vertexObject->GetIndices().size()/3 : 0).c_str());
			//ImGui::BulletText(fmt::format("Normals: {}", vertexObject->HasNormals() ? vertexObject->GetNormals().size() : 0).c_str());
			//ImGui::BulletText(fmt::format("UVs: {}", vertexObject->HasTexCoords() ? vertexObject->GetTexCoords().size() : 0).c_str());
			//ImGui::BulletText(fmt::format("Colors: {}", vertexObject->HasColors() ? vertexObject->GetColors().size() : 0).c_str());
			ImGui::Text("Visibility");

			ImGui::Checkbox("Solid", &renderingComponent->bRenderSolid);
			ImGui::SameLine();
			ImGui::Checkbox("Wireframe", &renderingComponent->bRenderWireframe);
			ImGui::SameLine();
			ImGui::Checkbox("Points", &renderingComponent->bRenderPoints);
			ImGui::SameLine();

			bool isDebugNormals = vertexObject->IsDebugNormals();
			if (ImGui::Checkbox("Normals", &isDebugNormals))
			{
				vertexObject->SetDebugNormals(isDebugNormals);
			}
			ImGui::SameLine();

			bool isSmoothShaded = (bool)renderingComponent->GetShadingType();
			if (ImGui::Checkbox("Smooth", &isSmoothShaded))
			{
				renderingComponent->SetShadingType((EShadingType)isSmoothShaded);
			}

			ImGui::Separator();

			ImGui::Text("Modifiers");
			ImGui::SameLine();
			if (ImGui::Button("Add Modifier"))
			{
				ImGui::OpenPopup("ModifierPopup");
			}
			if (ImGui::BeginPopup("ModifierPopup"))
			{
				ImGui::Text("Select a Modifier");
				ImGui::Separator();
				if (ImGui::Selectable("Subdivision Surface"))
				{
					auto subdivMod = make_unique<SubdivisionSurfaceModifier>(1);
					renderingComponent->AddModifier(std::move(subdivMod));
				}

				ImGui::EndPopup();
			}
			const std::vector<std::unique_ptr<IModifier>>& modifiers = renderingComponent->GetModifiers();
			bool wasModified = false;
			for (size_t i = 0; i < modifiers.size(); i++)
			{
				wasModified |= UILibrary::RenderModifier(modifiers[i].get(), i);
			}
			if (wasModified)
			{
				renderingComponent->RecalculateModifiers();
			}
			ImGui::Separator();

		}
	}

	void WObjectProperties::RenderCameraComponent(std::vector<SceneNode*> selectedNodes, CameraComponent* cameraComponent)
	{
		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
		{
			TracingCameraNode* tracingCameraNode = dynamic_cast<TracingCameraNode*>(cameraComponent->GetNodePtr());
			if (tracingCameraNode)
			{
				float FOV = cameraComponent->GetFOV();
				if (ImGui::DragFloat("FOV", &FOV, .1f, 5.0f, 130.0f))
				{
					cameraComponent->SetFOV(FOV);
				}
				if (ImGui::DragFloat("Focus Distance", &cameraComponent->FocusDistance, 0.01f, 1000.0f))
				{
					tracingCameraNode->RefreshDebugVisual();
				}
				ImGui::DragFloat("Aperture", &cameraComponent->Aperture, .01f, 0.0f, 10.0f);
			}
			ImGui::Separator();

		}
	}

	void WObjectProperties::RenderMaterialComponent(std::vector<SceneNode*> selectedNodes, MaterialComponent* materialComponent)
	{
		if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
		{
			Material& mat = materialComponent->GetMaterial();
			glm::vec3 albedo = mat.GetAlbedo();
			if (ImGui::ColorEdit3("Albedo", glm::value_ptr(albedo)))
			{
				mat.SetAlbedo(albedo);
			}
			float roughness = mat.GetRoughness();
			if (ImGui::SliderFloat("Roughness", &roughness, 0.001f, 1.0f))
			{
				mat.SetRoughness(roughness);
			}
			float metallic = mat.GetMetallic();
			if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f))
			{
				mat.SetMetallic(metallic);
			}
			float emittance = mat.GetEmittance();
			if (ImGui::SliderFloat("Emittance", &emittance, 0.0f, 50.0f))
			{
				mat.SetEmittance(emittance);
			}
			float IndexOfRefraction = mat.GetIndexOfRefraction();
			if (ImGui::SliderFloat("IndexOfRefraction", &IndexOfRefraction, 0.0f, 2.0f))
			{
				mat.SetIndexOfRefraction(IndexOfRefraction);
			}
			bool isTransparent = mat.IsTransparent();
			if (ImGui::Checkbox("Transparent", &isTransparent))
			{
				mat.SetTransparent(isTransparent);
			}
			ImGui::Separator();

		}
	}

}
