#include "namica/renderer/Renderer.h"
#include "namica/renderer/RendererConfig.h"
#include "namica/renderer/RendererCommand.h"
#include "namica/renderer/Renderer2D.h"

namespace Namica
{

void Renderer::init(RendererConfig const& _rendererConfig)
{
    RendererCommand::init(_rendererConfig.rendererAPIType);
    Renderer2D::init(_rendererConfig.renderer2DConfig);
}

void Renderer::shutdown()
{
    Renderer2D::shutdown();
}

void Renderer::setClearColor(glm::vec4 const& _clearColor)
{
    RendererCommand::setClearColor(_clearColor);
}

void Renderer::clear()
{
    RendererCommand::clear();
}

void Renderer::updateViewport(uint32_t _width, uint32_t _height)
{
    RendererCommand::updateViewport(_width, _height);
}

}  // namespace Namica