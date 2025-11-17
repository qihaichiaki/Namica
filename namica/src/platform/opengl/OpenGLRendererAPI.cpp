#include "platform/opengl/OpenGLRendererAPI.h"
#include "platform/opengl/OpenGLVertexArray.h"
#include "platform/opengl/OpenGLShader.h"
#include "namica/core/Log.h"

#include <glad/glad.h>

namespace Namica
{

static void APIENTRY onGLDebugCallback(GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       GLsizei length,
                                       GLchar const* message,
                                       void const* userParam)
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

void OpenGLRendererAPI::setClearColor(glm::vec4 const& _clearColor)
{
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
}

void OpenGLRendererAPI::clear()
{
    // 颜色区域和深度区域
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::drawIndexed(Ref<VertexArray> const& _vertexArray,
                                    Ref<Shader> const& _shader)
{
    OpenGLShader& vs{*refCast<OpenGLShader>(_shader)};
    vs.useProgram();
    OpenGLVertexArray const& va{*refCast<OpenGLVertexArray>(_vertexArray)};
    glBindVertexArray(va.m_rendererID);

    glDrawElements(GL_TRIANGLES, va.m_indexBuffer->getCount(), GL_UNSIGNED_INT, nullptr);
}

void OpenGLRendererAPI::drawIndexed(Ref<VertexArray> const& _vertexArray,
                                    Ref<Shader> const& _shader,
                                    uint32_t _indexCount)
{
    OpenGLShader& vs{*refCast<OpenGLShader>(_shader)};
    vs.useProgram();
    OpenGLVertexArray const& va{*refCast<OpenGLVertexArray>(_vertexArray)};
    glBindVertexArray(va.m_rendererID);

    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr);
}

void OpenGLRendererAPI::drawLines(Ref<VertexArray> const& _vertexArray,
                                  Ref<Shader> const& _shader,
                                  uint32_t _vertexCount)
{
    OpenGLShader& vs{*refCast<OpenGLShader>(_shader)};
    vs.useProgram();
    OpenGLVertexArray const& va{*refCast<OpenGLVertexArray>(_vertexArray)};
    glBindVertexArray(va.m_rendererID);

    glDrawArrays(GL_LINES, 0, _vertexCount);
}

void OpenGLRendererAPI::setLineWidth(float _width)
{
    glLineWidth(_width);
}

void OpenGLRendererAPI::updateViewport(uint32_t _width, uint32_t _height)
{
    glViewport(0, 0, _width, _height);
}

}  // namespace Namica