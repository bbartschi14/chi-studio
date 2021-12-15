#include "WTimeline.h"
#include "ChiGraphics/Application.h"
#include "ChiGraphics/Scene.h"
#include "ChiCore/UI/UILibrary.h"
#include "ChiGraphics/Keyframing/KeyframeTrack.h"
#include "ChiGraphics/Keyframing/Keyframe.h"
#include "ChiGraphics/Keyframing/Keyframeable.h"
#include "ChiGraphics/Keyframing/KeyframeManager.h"
#include <iostream>

namespace CHISTUDIO {

void WTimeline::Render(Application& InApplication, float InDeltaTime)
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

    ImGui::PushItemWidth(130);
    ImGui::DragIntRange2("Timeline Range", &sequence.FrameMin, &sequence.FrameMax, 1, 0, 0, "Start: %d", "End: %d");
    ImGui::SameLine();
    
    int currentFrame = KeyframeManager::GetInstance().GetCurrentFrame();
    if (ImGui::DragInt("Frame ", &currentFrame))
    {
        KeyframeManager::GetInstance().SetCurrentFrame(currentFrame);
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();

    if (!bIsPlayingPreview)
    {
        if (ImGui::Button("Play"))
        {
            bIsPlayingPreview = true;
            TimeSinceLastUpdate = 0.0f;
            if (currentFrame < PreviewStart)
            {
                currentFrame = PreviewStart;
                KeyframeManager::GetInstance().SetCurrentFrame(currentFrame);
            }
        }
    }
    else
    {
        TimeSinceLastUpdate += InDeltaTime;
        float timePerFrame = 1.0f / PreviewFPS;
        if (TimeSinceLastUpdate >= timePerFrame)
        {
            TimeSinceLastUpdate -= timePerFrame;
            currentFrame++;
            if (currentFrame > PreviewEnd)
            {
                currentFrame = PreviewStart;
            }
            KeyframeManager::GetInstance().SetCurrentFrame(currentFrame);
        }

        if (ImGui::Button("Pause"))
        {
            bIsPlayingPreview = false;
        }
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(130);
    ImGui::DragIntRange2("Preview Range", &PreviewStart, &PreviewEnd, 1, sequence.FrameMin, sequence.FrameMax, "Start: %d", "End: %d");

    std::vector<SceneNode*> selectedNodes = InApplication.GetSelectedNodes();

    std::vector<std::string> trackNames;
    std::vector<IKeyframeBase*> keyframes;
    
    bool bCanSequencerChangeFrame = true;
    // Currently only work with single select
    if (selectedNodes.size() == 1)
    {
        SceneNode* keyframeable = selectedNodes[0];
        std::vector<IKeyframeable*> keyframeables = keyframeable->GetKeyframeables();

        // Check indices
        if (keyframeable->SelectedKeyframeableIndex > keyframeables.size())
        {
            keyframeable->SelectedKeyframeableIndex = 0;
            selectedEntry = -1;
        }

        IKeyframeable* selectedKeyframeable = keyframeables[keyframeable->SelectedKeyframeableIndex];
        trackNames = selectedKeyframeable->GetKeyframeTrackNames();

        if (selectedKeyframeable->SelectedTrackIndex > trackNames.size())
        {
            selectedKeyframeable->SelectedTrackIndex = 0;
            selectedEntry = -1;
        }

        
        const char* currentTrackModeString = trackNames[selectedKeyframeable->SelectedTrackIndex].c_str();
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        if (ImGui::BeginCombo("##Mode", currentTrackModeString))
        {
            for (size_t keyframeableIndex = 0; keyframeableIndex < keyframeables.size(); keyframeableIndex++)
            {
                IKeyframeable* kf = keyframeables[keyframeableIndex];
                ImGui::Text(kf->Name.c_str());
                ImGui::Separator();
                std::vector<std::string> kfTrackNames = kf->GetKeyframeTrackNames();
                for (int i = 0; i < kfTrackNames.size(); i++)
                {
                    bool isSelected = selectedKeyframeable->SelectedTrackIndex == i && kf == selectedKeyframeable;
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                    if (ImGui::Selectable(kfTrackNames[i].c_str(), isSelected))
                    {
                        bCanSequencerChangeFrame = false;
                        kf->SelectedTrackIndex = i;
                        keyframeable->SelectedKeyframeableIndex = keyframeableIndex;
                    }
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0))
                    {
                        bCanSequencerChangeFrame = false;
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }
        ImGui::SameLine();
        ImGui::Text("Track");

        trackNames = selectedKeyframeable->GetKeyframeTrackNames();
        ImGui::SameLine();
        if (ImGui::Button("Add Key"))
        {
            selectedKeyframeable->CreateKeyframeOnTrack(trackNames[selectedKeyframeable->SelectedTrackIndex], currentFrame);
        }

        if (selectedEntry > -1)
        {
            ImGui::SameLine();
            if (ImGui::Button("Delete Key"))
            {
                selectedKeyframeable->DeleteKeyframeOnTrack(trackNames[selectedKeyframeable->SelectedTrackIndex], selectedEntry);
                selectedEntry = -1;
            }
        }
        
        keyframes = selectedKeyframeable->GetKeyframesOnTrack(trackNames[selectedKeyframeable->SelectedTrackIndex]);
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
    int flags = ImSequencer::SEQUENCER_EDIT_STARTEND;
    if (bCanSequencerChangeFrame)
    {
        flags = flags | ImSequencer::SEQUENCER_CHANGE_FRAME;
    }

    if (Sequencer(&sequence, &currentFrame, &expanded, &selectedEntry, &firstFrame, flags))
    {
        if (selectedEntry > -1)
        {
            keyframes[selectedEntry]->Frame = sequence.Items[selectedEntry].Frame;
            InApplication.UpdateToTimelineFrame(currentFrame);
        }
        if (currentFrame != previousCurrentFrame)
        {
            KeyframeManager::GetInstance().SetCurrentFrame(currentFrame);
        }
    }

    if (selectedEntry > -1)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 8, 8 });
        ImGui::Begin("Selected Key");
        if (keyframes[selectedEntry]->RenderBaseUI())
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