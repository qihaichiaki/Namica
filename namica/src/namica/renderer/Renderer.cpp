#include "namica/renderer/Renderer.h"
#include "namica/renderer/RendererConfig.h"
#include "namica/renderer/RendererCommand.h"
#include "namica/renderer/Renderer2D.h"
#include "namica/renderer/Framebuffer.h"

namespace Namica
{

static Ref<Framebuffer> s_framebuffer{nullptr};

void Renderer::init(RendererConfig const& _rendererConfig)
{
    RendererCommand::init(_rendererConfig.rendererAPIType);
    Renderer2D::init(_rendererConfig.renderer2DConfig);
}

void Renderer::shutdown()
{
    Renderer2D::shutdown();
    s_framebuffer = nullptr;
}

void Renderer::beginRender(glm::mat4 const& _cameraPV, Ref<Framebuffer> const& _frameBuffer)
{
    s_framebuffer = _frameBuffer;
    if (s_framebuffer)
    {
        s_framebuffer->bind();
    }
    Renderer2D::beginScene(_cameraPV);
}

void Renderer::endRender()
{
    Renderer2D::endScene();
    if (s_framebuffer)
    {
        s_framebuffer->unBind();
    }
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
    if (s_framebuffer)
    {
        s_framebuffer->resize(_width, _height);
        s_framebuffer->bind();
    }
    RendererCommand::updateViewport(_width, _height);
}

}  // namespace Namica