#include "KeyframeManager.h"
#include "ChiGraphics/Application.h"
namespace CHISTUDIO {

void KeyframeManager::SetCurrentFrame(int InFrame)
{
	CurrentFrame = InFrame;
	if (AppRef)
	{
		AppRef->UpdateToTimelineFrame(CurrentFrame);
	}
}

}
