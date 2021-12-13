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
static const char* SequencerItemTypeNames[] = { "Camera","Music", "ScreenEffect", "FadeIn", "Animation" };

struct MySequence : public ImSequencer::SequenceInterface
{
    // interface with sequencer

    virtual int GetFrameMin() const {
        return mFrameMin;
    }
    virtual int GetFrameMax() const {
        return mFrameMax;
    }
    virtual int GetItemCount() const { return (int)myItems.size(); }

    virtual int GetItemTypeCount() const { return sizeof(SequencerItemTypeNames) / sizeof(char*); }
    virtual const char* GetItemTypeName(int typeIndex) const { return SequencerItemTypeNames[typeIndex]; }
    virtual const char* GetItemLabel(int index) const
    {
        static char tmps[512];
        snprintf(tmps, 512, "[%02d] %s", index, SequencerItemTypeNames[myItems[index].mType]);
        return tmps;
    }

    virtual void Get(int index, int** start, int** end, int* type, unsigned int* color)
    {
        MySequenceItem& item = myItems[index];
        if (color)
            *color = 0xFFAA8080; // same color for everyone, return color based on type
        if (start)
            *start = &item.mFrameStart;
        if (end)
            *end = &item.mFrameEnd;
        if (type)
            *type = item.mType;
    }
    virtual void Add(int type) { myItems.push_back(MySequenceItem{ type, 0, 10 }); };
    virtual void Del(int index) { myItems.erase(myItems.begin() + index); }
    virtual void Duplicate(int index) { myItems.push_back(myItems[index]); }

    //virtual size_t GetCustomHeight(int index) { return myItems[index].mExpanded ? 300 : 0; }
    virtual size_t GetCustomHeight(int index) { return 0; }

    // my datas
    MySequence() : mFrameMin(0), mFrameMax(0) {}
    int mFrameMin, mFrameMax;
    struct MySequenceItem
    {
        int mType;
        int mFrameStart, mFrameEnd;
        //bool mExpanded;
    };
    std::vector<MySequenceItem> myItems;

    virtual void DoubleClick(int index) {
        //if (myItems[index].mExpanded)
        //{
        //    myItems[index].mExpanded = false;
        //    return;
        //}
        //for (auto& item : myItems)
        //    item.mExpanded = false;
        //myItems[index].mExpanded = !myItems[index].mExpanded;
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
	WTimeline();

	void Render(Application& InApplication) override;

private:
    MySequence sequence;
};

}