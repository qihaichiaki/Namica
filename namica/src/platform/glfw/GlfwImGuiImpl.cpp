#include "platform/glfw/GlfwImGuiImpl.h"
#include "platform/glfw/GlfwWindow.h"

#include <backends/imgui_impl_glfw.h>

namespace Namica
{

Scope<ImGuiImplWindow> ImGuiImplWindow::create()
{
    return createScope<GlfwImGuiImpl>();
}

void GlfwImGuiImpl::init(Window const& _windowHandle)
{
    GLFWwindow* handle = static_cast<Namica::GlfwWindow const&>(_windowHandle).getWindowHandle();
    // glfw后端初始化
    ImGui_ImplGlfw_InitForOpenGL(handle, true);
}

void GlfwImGuiImpl::shutdown()
{
    ImGui_ImplGlfw_Shutdown();
}

void GlfwImGuiImpl::newFrame()
{
    ImGui_ImplGlfw_NewFrame();
}

}  // namespace Namica