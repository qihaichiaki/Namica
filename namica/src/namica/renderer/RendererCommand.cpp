#include "namica/renderer/RendererCommand.h"
#include "namica/core/Log.h"
#include "platform/opengl/OpenGLRendererAPI.h"

namespace Namica
{

Scope<RendererAPI> RendererCommand::s_rendererApi{nullptr};

void RendererCommand::init(RendererAPIType _rendererAPIType)
{
    switch (_rendererAPIType)
    {
        case RendererAPIType::OpenGL:
            NAMICA_CORE_INFO("底层选择OpenGL作为渲染API");
            s_rendererApi = createScope<OpenGLRendererAPI>();
            break;
        default:
            NAMICA_CORE_WARN("当前未选择底层渲染API");
            break;
    }

    NAMICA_CORE_ASSERT(s_rendererApi);
}

void RendererCommand::setClearColor(glm::vec4 const& _clearColor)
{
    s_rendererApi->setClearColor(_clearColor);
}

void RendererCommand::clear()
{
    s_rendererApi->clear();
}

}  // namespace Namica