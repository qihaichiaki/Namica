#include "namica/renderer/Renderer.h"
#include "namica/renderer/RendererConfig.h"
#include "namica/renderer/RendererCommand.h"

namespace Namica
{

void Renderer::init(RendererConfig const& _rendererConfig)
{
    RendererCommand::init(_rendererConfig.rendererAPIType);
}

void Renderer::setClearColor(glm::vec4 const& _clearColor)
{
    RendererCommand::setClearColor(_clearColor);
}

void Renderer::clear()
{
    RendererCommand::clear();
}

}  // namespace Namica