#include "platform/opengl/OpenGLFramebuffer.h"
#include "namica/core/Log.h"
#include <glad/glad.h>

namespace Namica
{

static GLenum textureTarget(bool _isMultisampled)
{
    return _isMultisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

static void bindTexture(bool _isMultisampled, uint32_t _id)
{
    glBindTexture(textureTarget(_isMultisampled), _id);
}

static void createTextures(bool _isMultisampled, uint32_t* _outID, uint32_t _count)
{
    glCreateTextures(textureTarget(_isMultisampled), _count, _outID);
}

static bool isDepthFormat(FramebufferTextureConfig _textureConfig)
{
    return _textureConfig.textureFormat == FramebufferTextureFormat::DEPTH24_STENCIL8;
}

OpenGLFramebuffer::OpenGLFramebuffer(FramebufferConfig const& _config)
    : m_width{_config.width}, m_height{_config.height}, m_samples{_config.samples}
{
    // 分类
    for (auto& attachment : _config.framebufferTextureAttachment)
    {
        if (isDepthFormat(attachment))
        {
            m_depthAttachmentConfig = attachment;
        }
        else
        {
            m_colorAttachmentConfigs.emplace_back(attachment);
        }
    }

    invalidate();
}

static void attachColorTexture(uint32_t _id,
                               uint32_t _samples,
                               GLenum _internalFormat,
                               GLenum _format,
                               uint32_t _width,
                               uint32_t _height,
                               uint32_t _index)
{
    bool isMultisampled{_samples > 1};
    if (isMultisampled)
    {
        glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE, _samples, _internalFormat, _width, _height, GL_FALSE);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     _internalFormat,
                     _width,
                     _height,
                     0,
                     _format,
                     GL_UNSIGNED_BYTE,
                     nullptr);

        // filter and wrap
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // 上传到帧缓冲区
    // 贴图附件
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _index, textureTarget(isMultisampled), _id, 0);
}

static void attachDepthTexture(uint32_t _id,
                               uint32_t _samples,
                               GLenum _format,
                               GLenum _attachmentType,
                               uint32_t _width,
                               uint32_t _height)
{
    bool isMultisampled{_samples > 1};
    if (isMultisampled)
    {
        glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE, _samples, _format, _width, _height, GL_FALSE);
    }
    else
    {
        glTexStorage2D(GL_TEXTURE_2D, 1, _format, _width, _height);
    }

    // 上传到帧缓冲区
    glFramebufferTexture2D(GL_FRAMEBUFFER, _attachmentType, textureTarget(isMultisampled), _id, 0);
}

void OpenGLFramebuffer::clear()
{
    glDeleteFramebuffers(1, &m_rendererID);
    glDeleteTextures(static_cast<GLsizei>(m_colorAttachments.size()), m_colorAttachments.data());
    glDeleteTextures(1, &m_depthAttachment);

    m_colorAttachments.clear();
    m_depthAttachment = 0;
}

void OpenGLFramebuffer::invalidate()
{
    if (m_rendererID != 0)
    {
        // 如果需要重新附加, 需要将之前的帧缓冲区删除
        clear();
    }

    // 创建帧缓冲区
    glCreateFramebuffers(1, &m_rendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

    bool isMultisampled{m_samples > 1};

    // 创建颜色纹理
    if (m_colorAttachmentConfigs.size() > 0)
    {
        m_colorAttachments.resize(m_colorAttachmentConfigs.size());
        createTextures(
            isMultisampled, m_colorAttachments.data(), (uint32_t)m_colorAttachments.size());

        for (uint32_t index{0}; index < m_colorAttachmentConfigs.size(); ++index)
        {
            bindTexture(isMultisampled, m_colorAttachments[index]);
            // 根据附加格式进行添加
            switch (m_colorAttachmentConfigs[index].textureFormat)
            {
                case FramebufferTextureFormat::RGBA8:
                    attachColorTexture(m_colorAttachments[index],
                                       m_samples,
                                       GL_RGBA8,
                                       GL_RGBA,
                                       m_width,
                                       m_height,
                                       index);
                    break;
                case FramebufferTextureFormat::RED_INTEGER:
                    attachColorTexture(m_colorAttachments[index],
                                       m_samples,
                                       GL_R32I,
                                       GL_RED_INTEGER,
                                       m_width,
                                       m_height,
                                       index);
                    break;
                default:
                    NAMICA_CORE_ASSERT(false);
                    break;
            }
        }
    }

    // 创建深度纹理
    if (m_depthAttachmentConfig.textureFormat != FramebufferTextureFormat::None)
    {
        createTextures(isMultisampled, &m_depthAttachment, 1);
        bindTexture(isMultisampled, m_depthAttachment);
        switch (m_depthAttachmentConfig.textureFormat)
        {
            case FramebufferTextureFormat::DEPTH24_STENCIL8:
                attachDepthTexture(m_depthAttachment,
                                   m_samples,
                                   GL_DEPTH24_STENCIL8,
                                   GL_DEPTH_STENCIL_ATTACHMENT,
                                   m_width,
                                   m_height);
                break;
            default:
                NAMICA_CORE_ASSERT(false);
                break;
        }
    }

    if (m_colorAttachments.size() > 0)
    {
        NAMICA_CORE_ASSERT(m_colorAttachments.size() <= 4, "当前最多支持四个color附件!");
        GLenum buffers[4] = {
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
        glDrawBuffers(static_cast<GLsizei>(m_colorAttachments.size()), buffers);
    }
    else
    {
        // 只绘制depth
        glDrawBuffer(GL_NONE);
    }

    // 检查 帧缓冲区的状态是否完整
    NAMICA_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                       "Framebuffer is incomplete!");
    bindTexture(isMultisampled, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
    clear();
}

void OpenGLFramebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
}

void OpenGLFramebuffer::unBind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t OpenGLFramebuffer::getColorAttachmentRendererID(uint32_t _colorAttachmentIndex) const
{
    return m_colorAttachments[_colorAttachmentIndex];
}

int OpenGLFramebuffer::readPixel(uint32_t _attachmentIndex, int _x, int _y) const
{
    NAMICA_CORE_ASSERT(_attachmentIndex < m_colorAttachments.size(), "附加index越界!");
    glReadBuffer(GL_COLOR_ATTACHMENT0 + _attachmentIndex);
    int pixelData;
    glReadPixels(_x, _y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    return pixelData;
}

void OpenGLFramebuffer::clearAttachment(uint32_t _attachmentIndex, int _value) const
{
    NAMICA_CORE_ASSERT(_attachmentIndex < m_colorAttachments.size(), "附加index越界!");
    if (m_colorAttachmentConfigs[_attachmentIndex].textureFormat ==
        FramebufferTextureFormat::RED_INTEGER)
    {
        glClearTexImage(m_colorAttachments[_attachmentIndex], 0, GL_RED_INTEGER, GL_INT, &_value);
    }
}

uint32_t OpenGLFramebuffer::getWidth() const
{
    return m_width;
}

uint32_t OpenGLFramebuffer::getHeight() const
{
    return m_height;
}

static constexpr uint32_t s_maxFramebufferSize = 8192;

void OpenGLFramebuffer::resize(uint32_t _width, uint32_t _height)
{
    if (_width == 0 || _height == 0 || _width > s_maxFramebufferSize ||
        _height > s_maxFramebufferSize)
    {
        NAMICA_CORE_WARN("帧缓冲区接收了异常的调整大小: {0}, {1}", _width, _height);
        return;
    }
    m_width = _width;
    m_height = _height;
    invalidate();
}

}  // namespace Namica