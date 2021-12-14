#pragma once
#include "IWidget.h"
#include <vector>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include "external/src/ImGuizmo/ImGuizmo.h"
#include "external/src/ImGuizmo/ImSequencer.h"
#include "external/src/ImGuizmo/ImZoomSlider.h"
#include "external/src/ImGuizmo/ImCurveEdit.h"

namespace CHISTUDIO {

//
//
// ImSequencer interface
//
//

struct FSequence : public ImSequencer::SequenceInterface
{
    // interface with sequencer

    virtual int GetFrameMin() const {
        return FrameMin;
    }
    virtual int GetFrameMax() const {
        return FrameMax;
    }
    virtual int GetItemCount() const { return (int)Items.size(); }

    virtual int GetItemTypeCount() const { return 0; }
    virtual const char* GetItemTypeName(int typeIndex) const { return ""; }
    virtual const char* GetItemLabel(int index) const
    {
        static char tmps[512];
        snprintf(tmps, 512, "[%02d] %s", index, "");
        return tmps;
    }

    virtual void Get(int index, int** start, int** end, int* type, unsigned int* color)
    {
        FKeyframeItem& item = Items[index];
        if (color)
            *color = 0xFFAA8080; // same color for everyone, return color based on type
        if (start)
            *start = &item.Frame; 
        if (end)
            *end = &item.Frame;
        if (type)
            *type = item.Type;
    }
    //virtual void Add(int type) { Items.push_back(FKeyframeItem{ type, 0 }); };
    //virtual void Del(int index) { Items.erase(Items.begin() + index); }
    //virtual void Duplicate(int index) { Items.push_back(Items[index]); }

    //virtual size_t GetCustomHeight(int index) { return myItems[index].mExpanded ? 300 : 0; }
    virtual size_t GetCustomHeight(int index) { return 0; }

    FSequence() : FrameMin(0), FrameMax(0) {}
    int FrameMin, FrameMax;
    struct FKeyframeItem
    {
        int Type;
        int Frame;
    };
    std::vector<FKeyframeItem> Items;

    virtual void DoubleClick(int index) {
       
    }

    virtual void CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect)
    {
        
    }

    virtual void CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect)
    {

    }
};

class WTimeline : public IWidget
{
public:
    WTimeline() : bIsPlayingPreview(false), PreviewStart(0), PreviewEnd(100), PreviewFPS(24), TimeSinceLastUpdate(0.0f)
    {
        sequence.FrameMin = 0;
        sequence.FrameMax = 200;
    };

	void Render(Application& InApplication, float InDeltaTime) override;

private:
    FSequence sequence;
    bool bIsPlayingPreview;
    int PreviewStart;
    int PreviewEnd;
    int PreviewFPS;

    float TimeSinceLastUpdate;
};

}