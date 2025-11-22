#pragma once

#include "namica/core/Memory.h"
#include <vector>

namespace Namica
{

enum class FramebufferTextureFormat
{
    None,
    RGBA8,            // 颜色附件格式
    RED_INTEGER,      // int 类型附件格式, 方便读取id
    DEPTH24_STENCIL8  // 深度附件格式
};

struct FramebufferTextureConfig
{
    FramebufferTextureConfig() = default;
    FramebufferTextureConfig(FramebufferTextureFormat _textureFormat)
        : textureFormat{_textureFormat}
    {
    }

    FramebufferTextureFormat textureFormat{};
    // TODO: filtering/wrap
};

class FramebufferTextureAttachment
{
public:
    FramebufferTextureAttachment() = default;

    /**
     * @brief 添加不同的附件类型到帧缓冲区中, 需要注意帧缓冲区只接收一种DEPTH24_STENCIL8类型,
     * 如果出现重复的，则以最后一个深度缓冲区为准. 当前最多支持4种color附件(也就是说做多5个附件,
     * 第5个是深度附件)
     *
     * @param _attachments 附件类型们
     */
    FramebufferTextureAttachment(std::initializer_list<FramebufferTextureConfig> _attachments)
        : m_attachments{_attachments}
    {
    }

    std::vector<FramebufferTextureConfig>::iterator begin();
    std::vector<FramebufferTextureConfig>::iterator end();

    std::vector<FramebufferTextureConfig>::const_iterator begin() const;
    std::vector<FramebufferTextureConfig>::const_iterator end() const;

private:
    std::vector<FramebufferTextureConfig> m_attachments;
};

struct FramebufferConfig
{
    uint32_t width{1536};
    uint32_t height{960};
    uint32_t samples{1};  // 是否多重采样
    FramebufferTextureAttachment framebufferTextureAttachment{};
};

class Framebuffer
{
public:
    /**
     * @brief 获取帧缓冲区的颜色附加纹理的renderer id
     *
     * @param _colorAttachmentIndex 指定附加color附件的下标
     * @return uint32_t renderer id值
     */
    virtual uint32_t getColorAttachmentRendererID(uint32_t _colorAttachmentIndex) const = 0;

    /**
     * @brief 获取对应坐标的对应附加类型纹理内的值
     *
     * @param _attachmentIndex RED_INTEGER类型附件的下标
     * @param _x 坐标x
     * @param _y 坐标y
     * @return int 附件内的值int
     */
    virtual int readPixel(uint32_t _attachmentIndex, int _x, int _y) const = 0;

    /**
     * @brief 设置清除附加缓冲区的值
     *
     * @param _attachmentIndex
     * 附件类型缓冲区的下标(FramebufferTextureAttachment中对应类型的添加顺序)
     * @param _value RED_INTEGER类型附加缓冲区清除值
     */
    virtual void clearAttachment(uint32_t _attachmentIndex, int _value) const = 0;

    /// @brief 返回帧缓冲区的宽度
    virtual uint32_t getWidth() const = 0;

    /// @brief 返回帧缓冲区的高度
    virtual uint32_t getHeight() const = 0;

public:
    static Ref<Framebuffer> create(FramebufferConfig const& _config);

private:
    /// @brief 帧缓冲区绑定
    virtual void bind() = 0;

    /// @brief 帧缓冲区解绑
    virtual void unBind() = 0;

    /// @brief 帧缓冲区调整大小
    virtual void resize(uint32_t _width, uint32_t _height) = 0;

    friend class Renderer;
};
}  // namespace Namica