#include "platform/opengl/OpenGLRendererAPI.h"
#include "namica/core/Log.h"

#include <glad/glad.h>

namespace Namica
{

static void APIENTRY onGLDebugCallback(GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       GLsizei length,
                                       GLchar const *message,
                                       void const *userParam)
{
    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        NAMICA_CORE_ASSERT(false);
        NAMICA_CORE_ERROR("OpenGL调用出错: {0}", std::string{message, (uint32_t)length});
    }
}

void OpenGLRendererAPI::init()
{
    // 启用纹理混合
    glEnable(GL_BLEND);
    // 经典混合函数, 实现透明效果
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    // 优化线段渲染
    glEnable(GL_LINE_SMOOTH);

    // 注册debug日志回调
    glDebugMessageCallback(onGLDebugCallback, nullptr);
}

void OpenGLRendererAPI::setClearColor(glm::vec4 const &_clearColor)
{
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
}

void OpenGLRendererAPI::clear()
{
    // 颜色区域和深度区域
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

}  // namespace Namica