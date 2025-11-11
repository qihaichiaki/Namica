#include "platform/opengl/OpenGLContext.h"
#include "namica/core/Log.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Namica
{

OpenGLContext::OpenGLContext(GLFWwindow* _windowHandle) : m_windowHandle{_windowHandle}
{
    NAMICA_CORE_ASSERT(m_windowHandle != nullptr);
}

void OpenGLContext::init()
{
    // 让创建的window局部成为当前活动的
    glfwMakeContextCurrent(m_windowHandle);
    // glad加载所有现代opengl函数
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    NAMICA_CORE_ASSERT(status)

    // 输出当前上下文使用gpu信息:
    NAMICA_CORE_INFO("当前OpenGLContext使用GPU信息: ");
    NAMICA_CORE_INFO("[+=======================================================");
    NAMICA_CORE_INFO("|+ vendor:   {}", (char*)(glGetString(GL_VENDOR)));
    NAMICA_CORE_INFO("|+ renderer: {}", (char*)(glGetString(GL_RENDERER)));
    NAMICA_CORE_INFO("|+ version:  {}", (char*)(glGetString(GL_VERSION)));
    NAMICA_CORE_INFO("[+=======================================================");
}

}  // namespace Namica