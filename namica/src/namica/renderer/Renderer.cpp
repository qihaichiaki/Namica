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

    FramebufferConfig framebufferConfig{};
    // framebufferConfig.samples = 1;  // 1倍采样， 除开一倍采样其余均存在问题
    framebufferConfig.framebufferTextureAttachment = FramebufferTextureAttachment{
        FramebufferTextureFormat::RGBA8,            // 正常渲染的目标纹理
        FramebufferTextureFormat::RED_INTEGER,      // 需要为渲染对象添加的纹理标记ID
        FramebufferTextureFormat::DEPTH24_STENCIL8  // 深度缓冲区
    };
    s_framebuffer = Framebuffer::create(framebufferConfig);
}

void Renderer::shutdown()
{
    Renderer2D::shutdown();
    s_framebuffer = nullptr;
}

void Renderer::beginRender(glm::mat4 const& _cameraPV)
{
    s_framebuffer->bind();
    Renderer2D::beginScene(_cameraPV);
}

void Renderer::endRender()
{
    Renderer2D::endScene();
    s_framebuffer->unBind();
}

void Renderer::setClearColor(glm::vec4 const& _clearColor)
{
    RendererCommand::setClearColor(_clearColor);
}

void Renderer::clear()
{
    RendererCommand::clear();
    s_framebuffer->clearAttachment(1, -1);  // 纹理附加信息 清理为-1
}

void Renderer::updateViewport(uint32_t _width, uint32_t _height)
{
    s_framebuffer->resize(_width, _height);
    s_framebuffer->bind();
    RendererCommand::updateViewport(_width, _height);
}

uint32_t Renderer::getViewportWidth()
{
    return s_framebuffer->getWidth();
}

uint32_t Renderer::getViewportHeight()
{
    return s_framebuffer->getHeight();
}

uint32_t Renderer::getFramebufferRendererID()
{
    return s_framebuffer->getColorAttachmentRendererID(0);
}

}  // namespace Namica