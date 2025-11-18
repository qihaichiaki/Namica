#include "platform/opengl/OpenGLImGuiImpl.h"
#include <backends/imgui_impl_opengl3.h>

namespace Namica
{

void OpenGLImGuiImpl::init()
{
    ImGui_ImplOpenGL3_Init("#version 450");
}

void OpenGLImGuiImpl::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
}

void OpenGLImGuiImpl::newFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
}

void OpenGLImGuiImpl::drawData(ImDrawData* _data)
{
    ImGui_ImplOpenGL3_RenderDrawData(_data);
}

}  // namespace Namica