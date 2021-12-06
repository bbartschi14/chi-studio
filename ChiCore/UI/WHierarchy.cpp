#include "D:\GraphicsProjects\ChiStudio\ChiCore\UI\WHierarchy.h"
#include "ChiGraphics/Application.h"
#include "ChiGraphics/Scene.h"

namespace CHISTUDIO {
	WHierarchy::WHierarchy()
	{
	}

	void WHierarchy::Render(Application& InApplication)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

	ImGui::Begin("Hierarchy");

    Scene& scene = InApplication.GetScene();
    SceneNode& root = scene.GetRootNode();
    
    ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

    if (ImGui::BeginTable("HierarchyTable", 2, tableFlags))
    {
        // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 28.0f);
        ImGui::TableHeadersRow();

        // Load rows
        size_t childCount = root.GetChildrenCount();
        for (int i = 0; i < childCount; i++)
        {
            LoadRowsRecursively(root.GetChild(i), InApplication);
        }

        ImGui::EndTable();
    }

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
    {
        InApplication.SelectNode(nullptr, false);
    }

    // Hierarchy context menu
    bool openCylinderModal = false;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 4, 4 });
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))
        ImGui::OpenPopup("HierarchyContext");
    if (ImGui::BeginPopup("HierarchyContext"))
    {
        ImGui::Text("Options");
        ImGui::Separator();
        if (ImGui::Selectable("Add Cube"))
        {
            FDefaultObjectParams params;
            InApplication.CreatePrimitiveNode(EDefaultObject::Cube, params);
        }
        if (ImGui::Selectable("Add Plane"))
        {
            FDefaultObjectParams params;
            InApplication.CreatePrimitiveNode(EDefaultObject::Plane, params);
        }
        if (ImGui::Selectable("Add Cylinder"))
        {
            openCylinderModal = true;
        }
        if (ImGui::Selectable("Add Camera"))
        {
            InApplication.CreateCamera();
        }
        if (ImGui::Selectable("Add Point Light"))
        {
            InApplication.CreatePointLight();
        }
        if (ImGui::Selectable("Add Ambient Light"))
        {
            InApplication.CreateAmbientLight();
        }
        if (ImGui::Selectable("Add Tracing Sphere"))
        {
            InApplication.CreateTracingSphereNode();
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
    // ~ END Hierarchy context menu

	ImGui::End();
    ImGui::PopStyleVar();


    // Always center this window when appearing
    //ImVec2 center = ImGui::GetMainViewport()->GetCenter();
   // ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));


    if (openCylinderModal)
    {
        ImGui::OpenPopup("Create Cylinder");
    }
    if (ImGui::BeginPopupModal("Create Cylinder", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static int cylinderSideSegments = 6;
        ImGui::SliderInt("Side Segments", &cylinderSideSegments, 3, 100);
        ImGui::Separator();

        if (ImGui::Button("Create", ImVec2(120, 0))) 
        { 
            FDefaultObjectParams params;
            params.NumberOfSides = cylinderSideSegments;
            InApplication.CreatePrimitiveNode(EDefaultObject::Cylinder, params);
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }

        ImGui::EndPopup();
    }

    bool bDeleted = false;
    for (SceneNode* node : NodesToDelete)
    {
        node->GetParentPtr()->RemoveChild(node);
        if (node->IsSelected())
        {
            InApplication.SelectNode(nullptr, false);
        }
        bDeleted = true;
    }
    if (bDeleted)
    {
        NodesToDelete.clear();
    }
}

    void WHierarchy::LoadRowsRecursively(SceneNode& node, Application& app)
    {
        if (!node.IsHierarchyVisible()) return;

        size_t childCount = node.GetChildrenCount();

        // Create row for this node
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        const bool isFolderNode = (childCount > 0);

        ImGuiTreeNodeFlags folderFlags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
        ImGuiTreeNodeFlags leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;
        ImGuiTreeNodeFlags finalFlag = isFolderNode ? folderFlags : leafFlags;
        if (node.IsSelected()) 
        {
            finalFlag = finalFlag | ImGuiTreeNodeFlags_Selected;
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ 0.08f, 0.53f, 0.85f, 1.0f });
        }

        // Headers
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.025f, 0.43f, 0.75f, 1.0f });
        //colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        //colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        bool open = ImGui::TreeNodeEx(node.GetNodeName().c_str(), finalFlag);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 4, 4 });
        if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
        {
            ImGui::Text(node.GetNodeName().c_str(), "Options");
            ImGui::Separator();
            if (ImGui::Selectable("Delete"))
            {
                // Mark node for deletion
                NodesToDelete.push_back(&node);
            }

            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();

        ImGui::PopStyleColor();
        if (node.IsSelected())
        {
            ImGui::PopStyleColor();
        }

        if (ImGui::IsItemClicked())
        {
            if (ImGui::GetIO().KeyCtrl)
            {
                app.SelectNode(&node, true);
            }
            else
            {
                app.SelectNode(&node, false);
            }
        }

        ImGui::TableNextColumn();
        ImGui::TextUnformatted("Type");

        if (isFolderNode && open)
        {
            // Recurse into children
            for (size_t i = 0; i < childCount; i++) {
                SceneNode& child = node.GetChild(i);
                LoadRowsRecursively(child, app);
            }
            ImGui::TreePop();
        }
      
    }

    void WHierarchy::TestTable()
    {
        ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

        if (ImGui::BeginTable("3ways", 2, flags))
        {
            // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 28.0f);
            ImGui::TableHeadersRow();

            // Simple storage to output a dummy file-system.
            struct MyTreeNode
            {
                const char* Name;
                const char* Type;
                int             Size;
                int             ChildIdx;
                int             ChildCount;
                static void DisplayNode(const MyTreeNode* node, const MyTreeNode* all_nodes)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    const bool is_folder = (node->ChildCount > 0);
                    if (is_folder)
                    {
                        bool open = ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Selected);
                        if (ImGui::IsItemClicked()) std::cout << "Clicked" << std::endl;
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(node->Type);
                        if (open)
                        {
                            for (int child_n = 0; child_n < node->ChildCount; child_n++)
                                DisplayNode(&all_nodes[node->ChildIdx + child_n], all_nodes);
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(node->Type);
                    }
                }
            };
            static const MyTreeNode nodes[] =
            {
                { "Root",                         "Folder",       -1,       1, 3    }, // 0
                { "Music",                        "Folder",       -1,       4, 2    }, // 1
                { "Textures",                     "Folder",       -1,       6, 3    }, // 2
                { "desktop.ini",                  "System file",  1024,    -1,-1    }, // 3
                { "File1_a.wav",                  "Audio file",   123000,  -1,-1    }, // 4
                { "File1_b.wav",                  "Audio file",   456000,  -1,-1    }, // 5
                { "Image001.png",                 "Image file",   203128,  -1,-1    }, // 6
                { "Copy of Image001.png",         "Image file",   203256,  -1,-1    }, // 7
                { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
            };

            MyTreeNode::DisplayNode(&nodes[0], nodes);

            ImGui::EndTable();
        }
    }

}