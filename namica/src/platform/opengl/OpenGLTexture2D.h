#pragma once

#include "namica/renderer/Texture.h"

namespace Namica
{

typedef unsigned int GLenum;
class OpenGLTexture2D final : public Texture2D
{
public:
    OpenGLTexture2D(uint32_t _width, uint32_t _height);
    OpenGLTexture2D(std::string const& _path);
    ~OpenGLTexture2D();

    uint32_t getWidth() const override;
    uint32_t getHeight() const override;
    void bind(uint32_t _slot = 0) const override;
    void setData(void* _data, uint32_t _size) override;
    bool isEqual(Texture const& _other) override;
    uint32_t getRendererId() override;

private:
    // 创建纹理存储区域和设置相关属性
    void createTextureStorage();

private:
    uint32_t m_rendererID;
    std::string m_path;  // 文件路径保存

    uint32_t m_width;
    uint32_t m_height;

    GLenum m_internalFormat;  // 纹理存储类型
    GLenum m_dataFormat;      // 纹理上传类型
};
}  // namespace Namica