#include "namica/renderer/Renderer.h"
#include "namica/renderer/RendererConfig.h"
#include "namica/renderer/RendererCommand.h"
#include "namica/renderer/Renderer2D.h"

namespace Namica
{

void Renderer::init(RendererConfig const& _rendererConfig)
{
    RendererCommand::init(_rendererConfig.rendererAPIType);
    Renderer2D::init();
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