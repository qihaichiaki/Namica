#include "namica/renderer/Framebuffer.h"
#include "namica/renderer/RendererCommand.h"
#include "namica/core/Log.h"
#include "platform/opengl/OpenGLFramebuffer.h"

namespace Namica
{

std::vector<FramebufferTextureConfig>::iterator FramebufferTextureAttachment::begin()
{
    return m_attachments.begin();
}

std::vector<FramebufferTextureConfig>::iterator FramebufferTextureAttachment::end()
{
    return m_attachments.end();
}

std::vector<FramebufferTextureConfig>::const_iterator FramebufferTextureAttachment::begin() const
{
    return m_attachments.begin();
}

std::vector<FramebufferTextureConfig>::const_iterator FramebufferTextureAttachment::end() const
{
    return m_attachments.end();
}

Ref<Framebuffer> Framebuffer::create(FramebufferConfig const& _config)
{
    switch (RendererCommand::getRendererAPIType())
    {
        case RendererAPIType::OpenGL:
            return createRef<OpenGLFramebuffer>(_config);
        default:
            NAMICA_CORE_ASSERT(false);
            return nullptr;
    }
}

}  // namespace Namica