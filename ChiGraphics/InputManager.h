#pragma once

#include <functional>
#include "External.h"

namespace CHISTUDIO {

class InputManager {
public:
    // Singleton design pattern.
    // InputManager is initialized the first time GetInstance is called.
    static InputManager& GetInstance() {
        static InputManager _instance;
        return _instance;
    }

    void SetWindow(GLFWwindow* InWindow);

    InputManager(const InputManager&) = delete;
    void operator=(const InputManager&) = delete;

    // key should take values in
    // https://www.glfw.org/docs/latest/group__keys.html. Usually ASCII characters
    // are enough.
    bool IsKeyPressed(int key);

    bool IsKeyReleased(int key);

    glm::dvec2 GetCursorPosition();

    bool IsLeftMousePressed();
    bool IsRightMousePressed();
    bool IsMiddleMousePressed();

    double FetchAndResetMouseScroll();

    glm::ivec2 GetWindowSize() const;

    void SetInputBlocked(bool InIsBlocked);
    bool GetInputBlocked() const
    {
        return bIsInputBlocked;
    }

private:
    InputManager() {
    }
    ~InputManager();
    GLFWwindow* Window;
    double mouse_scroll_{ 0 };

    bool bIsInputBlocked;

};

}
