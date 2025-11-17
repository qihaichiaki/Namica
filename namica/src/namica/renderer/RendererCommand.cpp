#include "namica/renderer/RendererCommand.h"
#include "namica/core/Log.h"
#include "platform/opengl/OpenGLRendererAPI.h"

namespace Namica
{

Scope<RendererAPI> RendererCommand::s_rendererAPI{nullptr};
RendererAPIType RendererCommand::s_rendererAPIType{RendererAPIType::None};

void RendererCommand::init(RendererAPIType _rendererAPIType)
{
    switch (_rendererAPIType)
    {
        case RendererAPIType::OpenGL:
            NAMICA_CORE_INFO("底层选择OpenGL作为渲染API");
            s_rendererAPI = createScope<OpenGLRendererAPI>();
            s_rendererAPIType = RendererAPIType::OpenGL;
            break;
        default:
            NAMICA_CORE_WARN("当前未选择底层渲染API");
            break;
    }

    NAMICA_CORE_ASSERT(s_rendererAPI);
    s_rendererAPI->init();
}

RendererAPIType RendererCommand::getRendererAPIType() noexcept
{
    return s_rendererAPIType;
}

void RendererCommand::setClearColor(glm::vec4 const& _clearColor)
{
    s_rendererAPI->setClearColor(_clearColor);
}

void RendererCommand::clear()
{
    s_rendererAPI->clear();
}

void RendererCommand::drawIndexed(Ref<VertexArray> const& _vertexArray, Ref<Shader> const& _shader)
{
    s_rendererAPI->drawIndexed(_vertexArray, _shader);
}

void RendererCommand::drawIndexed(Ref<VertexArray> const& _vertexArray,
                                  Ref<Shader> const& _shader,
                                  uint32_t _indexCount)
{
    s_rendererAPI->drawIndexed(_vertexArray, _shader, _indexCount);
}

void RendererCommand::drawLines(Ref<VertexArray> const& _vertexArray,
                                Ref<Shader> const& _shader,
                                uint32_t _vertexCount)
{
    s_rendererAPI->drawLines(_vertexArray, _shader, _vertexCount);
}

void RendererCommand::setLineWidth(float _width)
{
    s_rendererAPI->setLineWidth(_width);
}

void RendererCommand::updateViewport(uint32_t _width, uint32_t _height)
{
    s_rendererAPI->updateViewport(_width, _height);
}

}  // namespace Namica