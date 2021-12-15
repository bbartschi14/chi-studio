#pragma once

#include <functional>
#include "ChiGraphics/External.h"
#include <memory>

namespace CHISTUDIO {
    // Singleton pattern class for timeline frames
    class KeyframeManager
    {
    public:
        static KeyframeManager& GetInstance() {
            static KeyframeManager instance;
            return instance;
        }

        KeyframeManager(const KeyframeManager&) = delete;
        void operator=(const KeyframeManager&) = delete;

        void SetCurrentFrame(int InFrame);
        int GetCurrentFrame() const { return CurrentFrame; }
        void SetAppRef(class Application* InApp) { AppRef = InApp; }
    private:
        int CurrentFrame;

        class Application* AppRef;
        KeyframeManager() : CurrentFrame(10), AppRef(nullptr) {}
        ~KeyframeManager() {}
    };
}