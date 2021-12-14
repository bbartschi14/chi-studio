#include "WTimeline.h"
#include "ChiGraphics/Application.h"
#include "ChiGraphics/Scene.h"
#include "ChiCore/UI/UILibrary.h"
#include "ChiGraphics/Keyframing/KeyframeTrack.h"
#include "ChiGraphics/Keyframing/Keyframe.h"
#include "ChiGraphics/Keyframing/Keyframeable.h"
#include <iostream>

namespace CHISTUDIO {

WTimeline::WTimeline()
{
    // sequence with default values
    sequence.FrameMin = 0;
    sequence.FrameMax = 1000;
}

void WTimeline::Render(Application& InApplication)
{
    if (sequence.GetItemCount() > 0)
        sequence.Items.clear();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 6, 6 });

    ImGui::Begin("Timeline");

    Scene& scene = InApplication.GetScene();
    SceneNode& root = scene.GetRootNode();

    static int selectedEntry = -1;
    static int firstFrame = 0;
    static bool expanded = true;
    static int currentFrame = 100;

    ImGui::PushItemWidth(130);
    ImGui::InputInt("Frame Min", &sequence.FrameMin);
    ImGui::SameLine();
    ImGui::InputInt("Frame ", &currentFrame);
    ImGui::SameLine();
    ImGui::InputInt("Frame Max", &sequence.FrameMax);
    ImGui::PopItemWidth();

    std::vector<SceneNode*> selectedNodes = InApplication.GetSelectedNodes();

    std::vector<std::string> trackNames;
    std::vector<IKeyframeBase*> keyframes;

    // Currently only work with single select
    if (selectedNodes.size() == 1)
    {
        Transform& keyframeable = selectedNodes[0]->GetTransform();
        trackNames = keyframeable.GetKeyframeTrackNames();

        
        const char* currentTrackModeString = trackNames[keyframeable.SelectedTrackIndex].c_str();
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        if (ImGui::BeginCombo("##Mode", currentTrackModeString))
        {
            for (int i = 0; i < trackNames.size(); i++)
            {
                bool isSelected = keyframeable.SelectedTrackIndex == i;
                if (ImGui::Selectable(trackNames[i].c_str(), isSelected))
                {
                    keyframeable.SelectedTrackIndex = i;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
        ImGui::SameLine();
        ImGui::Text("Track");

        ImGui::SameLine();
        if (ImGui::Button("Add Key"))
        {
            keyframeable.CreateKeyframeOnTrack(trackNames[keyframeable.SelectedTrackIndex], currentFrame);
        }
        
        keyframes = keyframeable.GetKeyframesOnTrack(trackNames[keyframeable.SelectedTrackIndex]);
    }

    for (auto keyframe : keyframes)
    {
        sequence.Items.push_back(FSequence::FKeyframeItem{ 0, keyframe->Frame });
    }

    if (selectedEntry >= keyframes.size())
    {
        // Reset selection when inputs are changed
        selectedEntry = -1;
    }

    int previousCurrentFrame = currentFrame;
    // Sequencer returns true when any changes are made
    if (Sequencer(&sequence, &currentFrame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_CHANGE_FRAME))
    {
        if (selectedEntry > -1)
        {
            keyframes[selectedEntry]->Frame = sequence.Items[selectedEntry].Frame;
            InApplication.UpdateToTimelineFrame(currentFrame);
        }
        if (currentFrame != previousCurrentFrame)
        {
            InApplication.UpdateToTimelineFrame(currentFrame);
        }
    }

    if (selectedEntry > -1)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 8, 8 });
        ImGui::Begin("Selected Key");
        if (keyframes[selectedEntry]->RenderUI())
        {
            InApplication.UpdateToTimelineFrame(currentFrame);
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

}