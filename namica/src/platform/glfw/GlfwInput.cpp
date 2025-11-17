#include "namica/core/Input.h"
#include "namica/core/Application.h"
#include "platform/glfw/GlfwWindow.h"
#include <GLFW/glfw3.h>

namespace Namica
{

bool Input::isKeyPressed(KeyCode keycode)
{
    GLFWwindow* handle = static_cast<Namica::GlfwWindow const&>(Application::get().getMainWindow())
                             .getWindowHandle();
    int state = glfwGetKey(handle, static_cast<int>(keycode));
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::isMouseButtonPressed(KeyCode button)
{
    GLFWwindow* handle = static_cast<Namica::GlfwWindow const&>(Application::get().getMainWindow())
                             .getWindowHandle();
    int state = glfwGetMouseButton(handle, static_cast<int>(button));
    return state == GLFW_PRESS;
}

std::pair<float, float> Input::getMousePosition()
{
    GLFWwindow* handle = static_cast<Namica::GlfwWindow const&>(Application::get().getMainWindow())
                             .getWindowHandle();
    double x_pos, y_pos;
    glfwGetCursorPos(handle, &x_pos, &y_pos);
    return {static_cast<float>(x_pos), static_cast<float>(y_pos)};
}

float Input::getMouseX()
{
    auto [x, _] = getMousePosition();
    return x;
}

float Input::getMouseY()
{
    auto [_, y] = getMousePosition();
    return y;
}

}  // namespace Namica