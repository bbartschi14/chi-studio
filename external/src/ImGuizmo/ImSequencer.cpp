// https://github.com/CedricGuillemet/ImGuizmo
// v 1.84 WIP
//
// The MIT License(MIT)
//
// Copyright(c) 2021 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "ImSequencer.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <cstdlib>
#include <vector>
#include <iostream>
namespace ImSequencer
{
#ifndef IMGUI_DEFINE_MATH_OPERATORS
   static ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
      return ImVec2(a.x + b.x, a.y + b.y);
   }
#endif
   static int DrawKeyframe(ImDrawList* draw_list, ImVec2 pos, const ImVec2 size, const ImVec2 offset, bool edited, bool canSelect)
   {
      int ret = 0;
      ImGuiIO& io = ImGui::GetIO();

      static const ImVec2 localOffsets[4] = { ImVec2(1,0), ImVec2(0,1), ImVec2(-1,0), ImVec2(0,-1) };
      ImVec2 offsets[4];
      for (int i = 0; i < 4; i++)
      {
         offsets[i] = ImVec2{ pos.x, pos.y } + ImVec2{ localOffsets[i].x * size.x, localOffsets[i].y * size.y } + offset;
      }

      const ImVec2 center = ImVec2{ pos.x, pos.y } + offset;
      const ImRect anchor(ImVec2(center.x - size.x, center.y - size.y), ImVec2(center.x + size.x, center.y + size.y));
      if (anchor.Contains(io.MousePos))
      {
         ret = 1;
         if (io.MouseDown[0] && canSelect)
            ret = 2;
      }
      if (edited)
      {
         draw_list->AddConvexPolyFilled(offsets, 4, 0xFF0080FF);
         draw_list->AddPolyline(offsets, 4, 0xFF000000, true, 1.0f);
      }
      else if (ret)
      {
         draw_list->AddConvexPolyFilled(offsets, 4, ImColor(200, 150, 0));
         draw_list->AddPolyline(offsets, 4, 0xFF000000, true, 1.0f);
      }
      else
      {
         draw_list->AddConvexPolyFilled(offsets, 4, 0xFFFFFFFF);
         draw_list->AddPolyline(offsets, 4, 0xFF000000, true, 1.0f);
      }

      return ret;
   }

   static bool SequencerAddDelButton(ImDrawList* draw_list, ImVec2 pos, bool add = true)
   {
      ImGuiIO& io = ImGui::GetIO();
      ImRect delRect(pos, ImVec2(pos.x + 16, pos.y + 16));
      bool overDel = delRect.Contains(io.MousePos);
      int delColor = overDel ? 0xFFAAAAAA : 0x77A3B2AA;
      float midy = pos.y + 16 / 2 - 0.5f;
      float midx = pos.x + 16 / 2 - 0.5f;
      draw_list->AddRect(delRect.Min, delRect.Max, delColor, 4);
      draw_list->AddLine(ImVec2(delRect.Min.x + 3, midy), ImVec2(delRect.Max.x - 3, midy), delColor, 2);
      if (add)
         draw_list->AddLine(ImVec2(midx, delRect.Min.y + 3), ImVec2(midx, delRect.Max.y - 3), delColor, 2);
      return overDel;
   }

   bool Sequencer(SequenceInterface* sequence, int* currentFrame, bool* expanded, int* selectedEntry, int* firstFrame, int sequenceOptions)
   {
      bool ret = false;
      ImGuiIO& io = ImGui::GetIO();
      int cx = (int)(io.MousePos.x);
      int cy = (int)(io.MousePos.y);
      static float framePixelWidth = 10.f;
      static float framePixelWidthTarget = 10.f;
      int legendWidth = 0;

      static int movingEntry = -1;
      static int movingPos = -1;
      static int movingPart = -1;
      int delEntry = -1;
      int dupEntry = -1;
      int ItemHeight = 20;

      bool popupOpened = false;
      int sequenceCount = sequence->GetItemCount();
     // if (!sequenceCount)
     //    return false;
      ImGui::BeginGroup();

      ImDrawList* draw_list = ImGui::GetWindowDrawList();
      ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
      ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available
      int firstFrameUsed = firstFrame ? *firstFrame : 0;

      int controlHeight = ImGui::GetContentRegionAvail().y - 50;
      int frameCount = ImMax(sequence->GetFrameMax() - sequence->GetFrameMin(), 1);

      static bool MovingScrollBar = false;
      static bool MovingCurrentFrame = false;
      struct CustomDraw
      {
         int index;
         ImRect customRect;
         ImRect legendRect;
         ImRect clippingRect;
         ImRect legendClippingRect;
      };
      ImVector<CustomDraw> customDraws;
      ImVector<CustomDraw> compactCustomDraws;
      // zoom in/out
      const int visibleFrameCount = (int)floorf((canvas_size.x - legendWidth) / framePixelWidth);
      const float barWidthRatio = ImMin(visibleFrameCount / (float)frameCount, 1.f);
      const float barWidthInPixels = barWidthRatio * (canvas_size.x - legendWidth);

      ImRect regionRect(canvas_pos, canvas_pos + canvas_size);

      static bool panningView = false;
      static ImVec2 panningViewSource;
      static int panningViewFrame;
      //std::cout << ImGui::IsWindowFocused() << " " << ImGui::GetCurrentWindow()->Name << std::endl;
      if (ImGui::IsWindowFocused()/* && io.KeyAlt*/ && io.MouseDown[2])
      {
         if (!panningView)
         {
            panningViewSource = io.MousePos;
            panningView = true;
            panningViewFrame = *firstFrame;
         }
         ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
         *firstFrame = panningViewFrame - int((io.MousePos.x - panningViewSource.x) / framePixelWidth);
         *firstFrame = ImClamp(*firstFrame, sequence->GetFrameMin(), sequence->GetFrameMax() - visibleFrameCount);
      }
      if (panningView && !io.MouseDown[2])
      {
         panningView = false;
      }
      framePixelWidthTarget = ImClamp(framePixelWidthTarget, 0.1f, 50.f);

      framePixelWidth = ImLerp(framePixelWidth, framePixelWidthTarget, 0.33f);

      frameCount = sequence->GetFrameMax() - sequence->GetFrameMin();
      if (visibleFrameCount >= frameCount && firstFrame)
         *firstFrame = sequence->GetFrameMin();


      // --
      if (expanded && !*expanded)
      {
         ImGui::InvisibleButton("canvas", ImVec2(canvas_size.x - canvas_pos.x, (float)ItemHeight));
         draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_size.x + canvas_pos.x, canvas_pos.y + ItemHeight), 0xFF3D3837, 0);
         char tmps[512];
         ImFormatString(tmps, IM_ARRAYSIZE(tmps), sequence->GetCollapseFmt(), frameCount, sequenceCount);
         draw_list->AddText(ImVec2(canvas_pos.x + 26, canvas_pos.y + 2), 0xFFFFFFFF, tmps);
      }
      else
      {
         bool hasScrollBar(true);

         // test scroll area
         ImVec2 headerSize(canvas_size.x, (float)ItemHeight);
         ImVec2 scrollBarSize(canvas_size.x, 14.f);
         ImGui::InvisibleButton("topBar", headerSize);
         draw_list->AddRectFilled(canvas_pos, canvas_pos + headerSize, 0xFFFF0000, 0);
         ImVec2 childFramePos = ImGui::GetCursorScreenPos();
         ImVec2 childFrameSize(canvas_size.x, canvas_size.y - 8.f - headerSize.y - (hasScrollBar ? scrollBarSize.y : 0));
         ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
         ImGui::BeginChildFrame(889, childFrameSize);
         sequence->focused = ImGui::IsWindowFocused();
         ImGui::InvisibleButton("contentBar", ImVec2(canvas_size.x, float(controlHeight)));
         const ImVec2 contentMin = ImGui::GetItemRectMin();
         const ImVec2 contentMax = ImGui::GetItemRectMax();
         const ImRect contentRect(contentMin, contentMax);
         const float contentHeight = contentMax.y - contentMin.y;

         // full background
         draw_list->AddRectFilled(canvas_pos, canvas_pos + canvas_size, 0xFF242424, 0);

         // current frame top
         ImRect topRect(ImVec2(canvas_pos.x + legendWidth, canvas_pos.y), ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + ItemHeight));

         if (!MovingCurrentFrame && !MovingScrollBar && movingEntry == -1 && sequenceOptions & SEQUENCER_CHANGE_FRAME && currentFrame && *currentFrame >= 0 && topRect.Contains(io.MousePos) && io.MouseDown[0])
         {
            MovingCurrentFrame = true;
         }
         if (MovingCurrentFrame)
         {
            if (frameCount)
            {
               *currentFrame = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;
               if (*currentFrame < sequence->GetFrameMin())
               {
                  *currentFrame = sequence->GetFrameMin();
               }
               
               if (*currentFrame >= sequence->GetFrameMax())
               {
                  *currentFrame = sequence->GetFrameMax();
               }

               ret = true;
            }
            if (!io.MouseDown[0])
               MovingCurrentFrame = false;
         }

         //header
         draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_size.x + canvas_pos.x, canvas_pos.y + ItemHeight), 0xFF3D3837, 0);
         if (sequenceOptions & SEQUENCER_ADD)
         {
            if (SequencerAddDelButton(draw_list, ImVec2(canvas_pos.x + legendWidth - ItemHeight, canvas_pos.y + 2), true) && io.MouseReleased[0])
               ImGui::OpenPopup("addEntry");

            if (ImGui::BeginPopup("addEntry"))
            {
               for (int i = 0; i < sequence->GetItemTypeCount(); i++)
                  if (ImGui::Selectable(sequence->GetItemTypeName(i)))
                  {
                     sequence->Add(i);
                     *selectedEntry = sequence->GetItemCount() - 1;
                  }

               ImGui::EndPopup();
               popupOpened = true;
            }
         }

         //header frame number and lines
         int modFrameCount = 10;
         int frameStep = 1;
         while ((modFrameCount * framePixelWidth) < 150)
         {
            modFrameCount *= 2;
            frameStep *= 2;
         };
         int halfModFrameCount = modFrameCount / 2;

         auto drawLine = [&](int i, int regionHeight) {
            bool baseIndex = ((i % modFrameCount) == 0) || (i == sequence->GetFrameMax() || i == sequence->GetFrameMin());
            bool halfIndex = (i % halfModFrameCount) == 0;
            int px = (int)canvas_pos.x + int(i * framePixelWidth) /*+ legendWidth*/ - int(firstFrameUsed * framePixelWidth);
            int tiretStart = baseIndex ? 4 : (halfIndex ? 10 : 14);
            int tiretEnd = baseIndex ? regionHeight : ItemHeight;

            if (px <= (canvas_size.x + canvas_pos.x) && px >= (canvas_pos.x /*+ legendWidth*/))
            {
               draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)tiretStart), ImVec2((float)px, canvas_pos.y + (float)tiretEnd - 1), 0xFF606060, 1);

               draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)ItemHeight), ImVec2((float)px, canvas_pos.y + (float)regionHeight - 1), 0x30606060, 1);
            }

            if (baseIndex && px > (canvas_pos.x /*+ legendWidth*/))
            {
               char tmps[512];
               ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", i);
               draw_list->AddText(ImVec2((float)px + 3.f, canvas_pos.y), 0xFFBBBBBB, tmps);
            }

         };

         auto drawLineContent = [&](int i, int /*regionHeight*/) {
            int px = (int)canvas_pos.x + int(i * framePixelWidth) /*+ legendWidth*/ - int(firstFrameUsed * framePixelWidth);
            int tiretStart = int(contentMin.y);
            int tiretEnd = int(contentMax.y);

            if (px <= (canvas_size.x + canvas_pos.x) && px >= (canvas_pos.x + legendWidth))
            {
               draw_list->AddLine(ImVec2(float(px), float(tiretStart)), ImVec2(float(px), float(tiretEnd)), 0x30606060, 1);
            }
         };
         for (int i = sequence->GetFrameMin(); i <= sequence->GetFrameMax(); i += frameStep)
         {
            drawLine(i, ItemHeight);
         }
         drawLine(sequence->GetFrameMin(), ItemHeight);
         drawLine(sequence->GetFrameMax(), ItemHeight);

         // draw item names in the legend rect on the left
         size_t customHeight = 0;
        
         // vertical frame lines in content area
         for (int i = sequence->GetFrameMin(); i <= sequence->GetFrameMax(); i += frameStep)
         {
            drawLineContent(i, int(contentHeight));
         }
         drawLineContent(sequence->GetFrameMin(), int(contentHeight));
         drawLineContent(sequence->GetFrameMax(), int(contentHeight));

         // selection
         bool selected = selectedEntry && (*selectedEntry >= 0);
         if (selected)
         {
            customHeight = 0;
            for (int i = 0; i < *selectedEntry; i++)
               customHeight += sequence->GetCustomHeight(i);;
         }

         // slots
         static bool overSelectedPoint = false;
         customHeight = 0;
         ImVec2 pos = ImVec2(contentMin.x - firstFrameUsed * framePixelWidth, contentMin.y );
         for (int i = 0; i < sequenceCount; i++)
         {
            int* start;
            sequence->Get(i, &start, NULL, NULL, NULL);
            ImVec2 finalPos = ImVec2{ pos.x - 3.0f + *start * framePixelWidth, pos.y + 14.0f };
            ImVec2 size = ImVec2{ 6,6 };
            bool wasFocused = (ImGui::IsWindowFocused());
            int result = DrawKeyframe(draw_list, finalPos, size, ImVec2{0,0}, *selectedEntry == i, ImGui::IsMouseClicked(0));
            if (wasFocused)   ImGui::SetWindowFocus("Timeline");

            if (result)
            {             
               if (result == 2)
               {
                  *selectedEntry = i;
                  overSelectedPoint = true;
               }
               else if (*selectedEntry == i)
               {
                  overSelectedPoint = true;
               }
            }
         }

         // move selection
         static bool pointsMoved = false;
         static ImVec2 mousePosOrigin;
         static int startingFrame;
         if (overSelectedPoint && io.MouseDown[0])
         {
            if ((fabsf(io.MouseDelta.x) > 0.f || fabsf(io.MouseDelta.y) > 0.f) && *selectedEntry > -1)
            {
               if (!pointsMoved)
               {
                  mousePosOrigin = io.MousePos;
                  int* start;
                  sequence->Get(*selectedEntry, &start, NULL, NULL, NULL);
                  startingFrame = *start;
               }
               pointsMoved = true;
               int frameDiff = (int)((io.MousePos.x - mousePosOrigin.x) / framePixelWidth);
               if (std::abs(frameDiff) > 0)
               {
                  int* start;
                  sequence->Get(*selectedEntry, &start, NULL, NULL, NULL);
                  int& startRef = *start;
                  startRef = startingFrame + frameDiff;
                  ret = true;
               }
            }
         }

         if (overSelectedPoint && !io.MouseDown[0])
         {
            overSelectedPoint = false;
            if (pointsMoved)
            {
               pointsMoved = false;
               ret = true;
            }
         }

         // cursor
         if (currentFrame && firstFrame && *currentFrame >= *firstFrame && *currentFrame <= sequence->GetFrameMax())
         {
            static const float cursorWidth = 2.f;
            float cursorOffset = contentMin.x + (*currentFrame - firstFrameUsed) * framePixelWidth + framePixelWidth / 2 - cursorWidth * .5f;
            cursorOffset -= 8.f;
            draw_list->AddLine(ImVec2(cursorOffset, canvas_pos.y), ImVec2(cursorOffset, contentMax.y), 0xA02A2AFF, cursorWidth);
            char tmps[512];
            ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", *currentFrame);
            draw_list->AddText(ImVec2(cursorOffset + 10, canvas_pos.y + 2), 0xFF2A2AFF, tmps);
         }

         //draw_list->PopClipRect();
         //draw_list->PopClipRect();

         for (auto& customDraw : customDraws)
            sequence->CustomDraw(customDraw.index, draw_list, customDraw.customRect, customDraw.legendRect, customDraw.clippingRect, customDraw.legendClippingRect);
         for (auto& customDraw : compactCustomDraws)
            sequence->CustomDrawCompact(customDraw.index, draw_list, customDraw.customRect, customDraw.clippingRect);

         // copy paste
         if (sequenceOptions & SEQUENCER_COPYPASTE)
         {
            ImRect rectCopy(ImVec2(contentMin.x + 100, canvas_pos.y + 2)
               , ImVec2(contentMin.x + 100 + 30, canvas_pos.y + ItemHeight - 2));
            bool inRectCopy = rectCopy.Contains(io.MousePos);
            unsigned int copyColor = inRectCopy ? 0xFF1080FF : 0xFF000000;
            draw_list->AddText(rectCopy.Min, copyColor, "Copy");

            ImRect rectPaste(ImVec2(contentMin.x + 140, canvas_pos.y + 2)
               , ImVec2(contentMin.x + 140 + 30, canvas_pos.y + ItemHeight - 2));
            bool inRectPaste = rectPaste.Contains(io.MousePos);
            unsigned int pasteColor = inRectPaste ? 0xFF1080FF : 0xFF000000;
            draw_list->AddText(rectPaste.Min, pasteColor, "Paste");

            if (inRectCopy && io.MouseReleased[0])
            {
               sequence->Copy();
            }
            if (inRectPaste && io.MouseReleased[0])
            {
               sequence->Paste();
            }
         }
         //

         ImGui::EndChildFrame();
         ImGui::PopStyleColor();
         if (hasScrollBar)
         {
            ImGui::InvisibleButton("scrollBar", scrollBarSize);
            ImVec2 scrollBarMin = ImGui::GetItemRectMin();
            ImVec2 scrollBarMax = ImGui::GetItemRectMax();

            // ratio = number of frames visible in control / number to total frames

            float startFrameOffset = ((float)(firstFrameUsed - sequence->GetFrameMin()) / (float)frameCount) * (canvas_size.x - legendWidth);
            ImVec2 scrollBarA(scrollBarMin.x + legendWidth, scrollBarMin.y - 2);
            ImVec2 scrollBarB(scrollBarMin.x + canvas_size.x, scrollBarMax.y - 1);
            draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF222222, 0);

            ImRect scrollBarRect(scrollBarA, scrollBarB);
            bool inScrollBar = scrollBarRect.Contains(io.MousePos);

            draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF101010, 8);


            ImVec2 scrollBarC(scrollBarMin.x + legendWidth + startFrameOffset, scrollBarMin.y);
            ImVec2 scrollBarD(scrollBarMin.x + legendWidth + barWidthInPixels + startFrameOffset, scrollBarMax.y - 2);
            draw_list->AddRectFilled(scrollBarC, scrollBarD, (inScrollBar || MovingScrollBar) ? 0xFF606060 : 0xFF505050, 6);

            ImRect barHandleLeft(scrollBarC, ImVec2(scrollBarC.x + 14, scrollBarD.y));
            ImRect barHandleRight(ImVec2(scrollBarD.x - 14, scrollBarC.y), scrollBarD);

            bool onLeft = barHandleLeft.Contains(io.MousePos);
            bool onRight = barHandleRight.Contains(io.MousePos);

            static bool sizingRBar = false;
            static bool sizingLBar = false;

            draw_list->AddRectFilled(barHandleLeft.Min, barHandleLeft.Max, (onLeft || sizingLBar) ? 0xFFAAAAAA : 0xFF666666, 6);
            draw_list->AddRectFilled(barHandleRight.Min, barHandleRight.Max, (onRight || sizingRBar) ? 0xFFAAAAAA : 0xFF666666, 6);

            ImRect scrollBarThumb(scrollBarC, scrollBarD);
            static const float MinBarWidth = 44.f;
            if (sizingRBar)
            {
               if (!io.MouseDown[0])
               {
                  sizingRBar = false;
               }
               else
               {
                  float barNewWidth = ImMax(barWidthInPixels + io.MouseDelta.x, MinBarWidth);
                  float barRatio = barNewWidth / barWidthInPixels;
                  framePixelWidthTarget = framePixelWidth = framePixelWidth / barRatio;
                  int newVisibleFrameCount = int((canvas_size.x - legendWidth) / framePixelWidthTarget);
                  int lastFrame = *firstFrame + newVisibleFrameCount;
                  if (lastFrame > sequence->GetFrameMax())
                  {
                     framePixelWidthTarget = framePixelWidth = (canvas_size.x - legendWidth) / float(sequence->GetFrameMax() - *firstFrame);
                  }
               }
            }
            else if (sizingLBar)
            {
               if (!io.MouseDown[0])
               {
                  sizingLBar = false;
               }
               else
               {
                  if (fabsf(io.MouseDelta.x) > FLT_EPSILON)
                  {
                     float barNewWidth = ImMax(barWidthInPixels - io.MouseDelta.x, MinBarWidth);
                     float barRatio = barNewWidth / barWidthInPixels;
                     float previousFramePixelWidthTarget = framePixelWidthTarget;
                     framePixelWidthTarget = framePixelWidth = framePixelWidth / barRatio;
                     int newVisibleFrameCount = int(visibleFrameCount / barRatio);
                     int newFirstFrame = *firstFrame + newVisibleFrameCount - visibleFrameCount;
                     newFirstFrame = ImClamp(newFirstFrame, sequence->GetFrameMin(), ImMax(sequence->GetFrameMax() - visibleFrameCount, sequence->GetFrameMin()));
                     if (newFirstFrame == *firstFrame)
                     {
                        framePixelWidth = framePixelWidthTarget = previousFramePixelWidthTarget;
                     }
                     else
                     {
                        *firstFrame = newFirstFrame;
                     }
                  }
               }
            }
            else
            {
               if (MovingScrollBar)
               {
                  if (!io.MouseDown[0])
                  {
                     MovingScrollBar = false;
                  }
                  else
                  {
                     float framesPerPixelInBar = barWidthInPixels / (float)visibleFrameCount;
                     *firstFrame = int((io.MousePos.x - panningViewSource.x) / framesPerPixelInBar) - panningViewFrame;
                     *firstFrame = ImClamp(*firstFrame, sequence->GetFrameMin(), ImMax(sequence->GetFrameMax() - visibleFrameCount, sequence->GetFrameMin()));
                  }
               }
               else
               {
                  if (scrollBarThumb.Contains(io.MousePos) && ImGui::IsMouseClicked(0) && firstFrame && !MovingCurrentFrame && movingEntry == -1)
                  {
                     MovingScrollBar = true;
                     panningViewSource = io.MousePos;
                     panningViewFrame = -*firstFrame;
                  }
                  if (!sizingRBar && onRight && ImGui::IsMouseClicked(0))
                     sizingRBar = true;
                  if (!sizingLBar && onLeft && ImGui::IsMouseClicked(0))
                     sizingLBar = true;

               }
            }
         }
      }

      ImGui::EndGroup();

      if (regionRect.Contains(io.MousePos))
      {
         bool overCustomDraw = false;
         for (auto& custom : customDraws)
         {
            if (custom.customRect.Contains(io.MousePos))
            {
               overCustomDraw = true;
            }
         }
         if (overCustomDraw)
         {
         }
         else
         {
#if 0
            frameOverCursor = *firstFrame + (int)(visibleFrameCount * ((io.MousePos.x - (float)legendWidth - canvas_pos.x) / (canvas_size.x - legendWidth)));
            //frameOverCursor = max(min(*firstFrame - visibleFrameCount / 2, frameCount - visibleFrameCount), 0);

            /**firstFrame -= frameOverCursor;
            *firstFrame *= framePixelWidthTarget / framePixelWidth;
            *firstFrame += frameOverCursor;*/
            if (io.MouseWheel < -FLT_EPSILON)
            {
               *firstFrame -= frameOverCursor;
               *firstFrame = int(*firstFrame * 1.1f);
               framePixelWidthTarget *= 0.9f;
               *firstFrame += frameOverCursor;
            }

            if (io.MouseWheel > FLT_EPSILON)
            {
               *firstFrame -= frameOverCursor;
               *firstFrame = int(*firstFrame * 0.9f);
               framePixelWidthTarget *= 1.1f;
               *firstFrame += frameOverCursor;
            }
#endif
         }
      }

      //if (expanded)
      //{
      //   bool overExpanded = SequencerAddDelButton(draw_list, ImVec2(canvas_pos.x + 2, canvas_pos.y + 2), !*expanded);
      //   if (overExpanded && io.MouseReleased[0])
      //      *expanded = !*expanded;
      //}

      if (delEntry != -1)
      {
         sequence->Del(delEntry);
         if (selectedEntry && (*selectedEntry == delEntry || *selectedEntry >= sequence->GetItemCount()))
            *selectedEntry = -1;
      }

      if (dupEntry != -1)
      {
         sequence->Duplicate(dupEntry);
      }
      return ret;
   }
}
