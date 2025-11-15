#include "platform/opengl/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Namica
{

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
}

void OpenGLRendererAPI::setClearColor(glm::vec4 const& _clearColor)
{
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
}

void OpenGLRendererAPI::clear()
{
    // 颜色区域和深度区域
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

}  // namespace Namica