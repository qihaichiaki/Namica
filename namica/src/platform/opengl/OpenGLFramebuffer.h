#pragma once

#include "namica/renderer/Framebuffer.h"

namespace Namica
{
class OpenGLFramebuffer final : public Framebuffer
{
public:
    OpenGLFramebuffer(FramebufferConfig const& _config);
    ~OpenGLFramebuffer();

    virtual uint32_t getColorAttachmentRendererID(uint32_t _colorAttachmentIndex) const override;
    virtual int readPixel(uint32_t _attachmentIndex, int _x, int _y) const override;
    virtual void clearAttachment(uint32_t _attachmentIndex, int _value) const override;
    virtual uint32_t getWidth() const override;
    virtual uint32_t getHeight() const override;

private:
    virtual void bind() override;
    virtual void unBind() override;
    virtual void resize(uint32_t _width, uint32_t _height) override;

    void clear();
    void invalidate();

private:
    uint32_t m_rendererID{0};  // 帧缓冲区ID
    uint32_t m_width{0};
    uint32_t m_height{0};
    uint32_t m_samples{1};

    std::vector<FramebufferTextureConfig> m_colorAttachmentConfigs;
    FramebufferTextureConfig m_depthAttachmentConfig;

    std::vector<uint32_t> m_colorAttachments{0};
    uint32_t m_depthAttachment{0};
};

}  // namespace Namica