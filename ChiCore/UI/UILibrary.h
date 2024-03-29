#pragma once
#include <windows.h>
#include <string>
#include <glm/glm.hpp>
#include "ChiGraphics/External.h"
#include <imgui_internal.h>
#include <functional>
#include "ChiGraphics/Modifiers/Modifier.h"
#include "ChiGraphics/Textures/ImageModifier.h"
#include "ChiGraphics/Textures/ImageManager.h"
#include "ChiGraphics/Textures/FImage.h"
#include <core.h>

namespace CHISTUDIO {

	class UILibrary
	{
	public:
		static bool DrawVector3Control(const std::string& InLabel, glm::vec3& InValues, float InSensitivity, float InResetValue = 0.0f, float InColumWidth = 100.0f)
		{
			bool dummyOut = false;
			return DrawVector3ControlWithStates(InLabel, InValues, dummyOut, InSensitivity, InResetValue, InColumWidth);
		}
		static bool DrawVector3ControlVertical(const std::string& InLabel, glm::vec3& InValues, float InSensitivity, float InResetValue = 0.0f, float InColumWidth = 100.0f)
		{
			bool dummyOut = false;
			return DrawVector3ControlWithStatesVertical(InLabel, InValues, dummyOut, InSensitivity, InResetValue, InColumWidth);
		}
		static bool DrawVector3ControlWithStatesVertical(const std::string& InLabel, glm::vec3& InValues, bool& OutWasDeactivated, float InSensitivity, float InResetValue = 0.0f, float InColumWidth = 100.0f)
		{
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			bool bWasChanged = false;

			ImGui::PushID(InLabel.c_str());

			// First column is  the label
			ImGui::Text(InLabel.c_str());

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth() * 3);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("X", buttonSize))
			{
				InValues.x = InResetValue;
				bWasChanged = true;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			bWasChanged |= ImGui::DragFloat("##X", &InValues.x, InSensitivity, 0.0f, 0.0f, "%.2f");
			OutWasDeactivated |= ImGui::IsItemDeactivatedAfterEdit();
			ImGui::PopItemWidth();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Y", buttonSize))
			{
				InValues.y = InResetValue;
				bWasChanged = true;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			bWasChanged |= ImGui::DragFloat("##Y", &InValues.y, InSensitivity, 0.0f, 0.0f, "%.2f");
			OutWasDeactivated |= ImGui::IsItemDeactivatedAfterEdit();
			ImGui::PopItemWidth();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Z", buttonSize))
			{
				InValues.z = InResetValue;
				bWasChanged = true;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			bWasChanged |= ImGui::DragFloat("##Z", &InValues.z, InSensitivity, 0.0f, 0.0f, "%.2f");
			OutWasDeactivated |= ImGui::IsItemDeactivatedAfterEdit();
			ImGui::PopItemWidth();
			ImGui::PopStyleVar();
			ImGui::Columns(1);

			ImGui::PopID();

			return (bWasChanged);
		}
		static bool DrawVector3ControlWithStates(const std::string& InLabel, glm::vec3& InValues, bool& OutWasDeactivated, float InSensitivity, float InResetValue = 0.0f, float InColumWidth = 100.0f)
		{
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			bool bWasChanged = false;

			ImGui::PushID(InLabel.c_str());
			ImGui::Columns(2);

			// First column is  the label
			ImGui::SetColumnWidth(0, InColumWidth);
			ImGui::Text(InLabel.c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("X", buttonSize))
			{
				InValues.x = InResetValue;
				bWasChanged = true;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			bWasChanged |= ImGui::DragFloat("##X", &InValues.x, InSensitivity, 0.0f, 0.0f, "%.2f");
			OutWasDeactivated |= ImGui::IsItemDeactivatedAfterEdit();
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Y", buttonSize))
			{
				InValues.y = InResetValue;
				bWasChanged = true;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			bWasChanged |= ImGui::DragFloat("##Y", &InValues.y, InSensitivity, 0.0f, 0.0f, "%.2f");
			OutWasDeactivated |= ImGui::IsItemDeactivatedAfterEdit();
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Z", buttonSize))
			{
				InValues.z = InResetValue;
				bWasChanged = true;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			bWasChanged |= ImGui::DragFloat("##Z", &InValues.z, InSensitivity, 0.0f, 0.0f, "%.2f");
			OutWasDeactivated |= ImGui::IsItemDeactivatedAfterEdit();
			ImGui::PopItemWidth();
			ImGui::PopStyleVar();
			ImGui::Columns(1);

			ImGui::PopID();

			return (bWasChanged);
		}

		static std::string PickFileName(char* filter = "All Files (*.*)\0*.*\0", HWND owner = NULL) {
			OPENFILENAME ofn;
			char fileName[MAX_PATH] = "";
			ZeroMemory(&ofn, sizeof(ofn));

			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = owner;
			ofn.lpstrFilter = filter;
			ofn.lpstrFile = fileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ofn.lpstrDefExt = "";

			std::string fileNameStr;

			if (GetOpenFileName(&ofn))
				fileNameStr = fileName;

			return fileNameStr;
		}

		static std::string SaveFileName(char* filter = "All Files (*.*)\0*.*\0", HWND owner = NULL) {
			OPENFILENAME ofn;
			char fileName[MAX_PATH] = "";
			ZeroMemory(&ofn, sizeof(ofn));

			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = owner;
			ofn.lpstrFilter = filter;
			ofn.lpstrFile = fileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ofn.lpstrDefExt = "";

			std::string fileNameStr;

			if (GetSaveFileName(&ofn))
				fileNameStr = fileName;

			return fileNameStr;
		}

		// Returns true if the modifier had any properties changed
		static bool RenderModifier(IModifier* InModifier, int count, int &indexToRemove)
		{
				ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
				ImGui::BeginChild(fmt::format("Modifier{}", count).c_str(), ImVec2(0, 20 + InModifier->GetUIHeight()), true, window_flags);
				
				bool wasModified = false;

				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu(InModifier->GetName().c_str()))
					{
						if (ImGui::MenuItem("Delete")) { wasModified = true; indexToRemove = count; }
						//if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}
				
				wasModified |= InModifier->RenderUI();

				ImGui::EndChild();
				ImGui::PopStyleVar();
				
				return wasModified;
		}

		// Returns true if the modifier had any properties changed
		static bool RenderImageModifier(IImageModifier* InModifier, int count, int& indexToRemove)
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			ImGui::BeginChild(fmt::format("Modifier{}", count).c_str(), ImVec2(0, 20 + InModifier->GetUIHeight()), true, window_flags);

			bool wasModified = false;

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(InModifier->GetName().c_str()))
				{
					if (ImGui::MenuItem("Delete")) { wasModified = true; indexToRemove = count; }
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			wasModified |= InModifier->RenderUI();

			ImGui::EndChild();
			ImGui::PopStyleVar();

			return wasModified;
		}

		static bool ImageSelector(const std::string& InLabel, FImage*& InImage)
		{
			bool wasModified = false;
		
			// ComboBox
			auto images = ImageManager::GetInstance().GetImages();
			std::string currentImageName = ImageManager::GetInstance().GetNameOfImage(InImage);
			if (ImGui::BeginCombo(("##ImageSelector" + InLabel).c_str(), currentImageName.c_str()))
			{
				for (int i = 0; i < images.size(); i++)
				{
					bool isSelected = currentImageName == images[i].first;
					if (ImGui::Selectable(images[i].first.c_str(), isSelected))
					{
						currentImageName = images[i].first;
						InImage = images[i].second;
						wasModified = true;
					}
		
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
		
				ImGui::EndCombo();
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				InImage = nullptr;
				wasModified = true;
			}
		
			return wasModified;
		}
	};

}