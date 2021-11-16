#include "InputManager.h"
#include <stdexcept>
#include "GLFW/glfw3.h"
#include <cassert>

namespace CHISTUDIO {

void InputManager::SetWindow(GLFWwindow* window) {
	assert(Window == nullptr);
	Window = window;
	glfwSetScrollCallback(
		window, [](GLFWwindow* window, double xoffset, double yoffset) {
			if (!InputManager::GetInstance().GetInputBlocked())
				InputManager::GetInstance().mouse_scroll_ += yoffset;
		});
}

bool InputManager::IsKeyPressed(int key)
{
	return glfwGetKey(Window, key) == GLFW_PRESS && !bIsInputBlocked;
}

bool InputManager::IsKeyReleased(int key)
{
	return glfwGetKey(Window, key) == GLFW_RELEASE && !bIsInputBlocked;
}

glm::dvec2 InputManager::GetCursorPosition()
{
	double xpos, ypos;
	glfwGetCursorPos(Window, &xpos, &ypos);
	return glm::dvec2(xpos, ypos);
}

bool InputManager::IsLeftMousePressed()
{
	return glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !bIsInputBlocked;
}
bool InputManager::IsRightMousePressed()
{
	return glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !bIsInputBlocked;

}
bool InputManager::IsMiddleMousePressed()
{
	return glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS && !bIsInputBlocked;
}

double InputManager::FetchAndResetMouseScroll()
{
	auto ret = mouse_scroll_;
	mouse_scroll_ = 0.0;
	return ret;
}

glm::ivec2 InputManager::GetWindowSize() const
{
	int width, height;
	glfwGetFramebufferSize(Window, &width, &height);
	return glm::ivec2(width, height);
}

void InputManager::SetInputBlocked(bool InIsBlocked)
{
	bIsInputBlocked = InIsBlocked;
}

InputManager::~InputManager()
{
	if (Window != nullptr) {
		// Unregister
		glfwSetScrollCallback(Window, nullptr);
	}
}

}