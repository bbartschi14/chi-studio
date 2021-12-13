#include "WTimeline.h"
#include "ChiGraphics/Application.h"
#include "ChiGraphics/Scene.h"
#include "ChiCore/UI/UILibrary.h"

namespace CHISTUDIO {

WTimeline::WTimeline()
{
    // sequence with default values
    sequence.mFrameMin = -100;
    sequence.mFrameMax = 1000;
    sequence.myItems.push_back(MySequence::MySequenceItem{ 0, 10, 30});
    sequence.myItems.push_back(MySequence::MySequenceItem{ 1, 20, 30});
    sequence.myItems.push_back(MySequence::MySequenceItem{ 3, 12, 60 });
    sequence.myItems.push_back(MySequence::MySequenceItem{ 2, 61, 90 });
    sequence.myItems.push_back(MySequence::MySequenceItem{ 4, 90, 99 });
}

void WTimeline::Render(Application& InApplication)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

    ImGui::Begin("Timeline");

    Scene& scene = InApplication.GetScene();
    SceneNode& root = scene.GetRootNode();

    // let's create the sequencer
    static int selectedEntry = -1;
    static int firstFrame = 0;
    static bool expanded = true;
    static int currentFrame = 100;

    ImGui::PushItemWidth(130);
    ImGui::InputInt("Frame Min", &sequence.mFrameMin);
    ImGui::SameLine();
    ImGui::InputInt("Frame ", &currentFrame);
    ImGui::SameLine();
    ImGui::InputInt("Frame Max", &sequence.mFrameMax);
    ImGui::PopItemWidth();
    Sequencer(&sequence, &currentFrame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_CHANGE_FRAME);
    // add a UI to edit that particular item
    //if (selectedEntry != -1)
    //{
    //    const MySequence::MySequenceItem& item = sequence.myItems[selectedEntry];
    //    ImGui::Text("I am a %s, please edit me", SequencerItemTypeNames[item.mType]);
    //    // switch (type) ....
    //}

    ImGui::End();
    ImGui::PopStyleVar();
}

}