#pragma once

#include <functional>
#include "ChiGraphics/External.h"
#include <memory>

namespace CHISTUDIO {
    /** Singleton pattern class for managing timeline frames.
     * Allows for external classes to modify the current timeline without accessing the UI. For
     * example, the rendering panel adjusts the keyframe when rendering multi-frame animations.
     */
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