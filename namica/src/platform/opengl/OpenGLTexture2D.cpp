#include "platform/opengl/OpenGLTexture2D.h"
#include "namica/core/Log.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Namica
{

// 纹理属性相关通用设置
void OpenGLTexture2D::createTextureStorage()
{
    // 创建存储纹理的区域
    glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);
    // 设置纹理参数, 这些未来需要暴露给外面进行设置
    // 设置纹理属性
    glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // 线性插值
    // 放大纹理过滤器设置
    glTextureParameteri(m_rendererID,
                        GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR);  // 非线性插值, 防止模糊 - 默认线性插值
    // 如果超出纹理坐标的映射(0, 1), 则进行重复利用
    glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

OpenGLTexture2D::OpenGLTexture2D(uint32_t _width, uint32_t _height)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
    m_internalFormat = GL_RGBA8;
    m_dataFormat = GL_RGBA;

    // 创建纹理存储
    glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);
    createTextureStorage();
}

OpenGLTexture2D::OpenGLTexture2D(std::string const& _path)
{
    // 翻转读取图片缓冲区, 因为opengl的模式是左下角为0, 0. 默认是左上角
    stbi_set_flip_vertically_on_load(1);
    int width, height;
    int channels;  // 通道数

    stbi_uc* data = nullptr;
    // TODO: 后续针对于stbi_load读取到的data可以进行缓存, 类似于shader的数据
    data = stbi_load(m_path.c_str(), &width, &height, &channels, 0);
    NAMICA_CORE_ASSERT(data, "OpenGLTexture2D 加载纹理失败!");
    m_width = width;
    m_height = height;

    if (channels == 4)  // rgba
    {
        m_internalFormat = GL_RGBA8;
        m_dataFormat = GL_RGBA;
    }
    else if (channels == 3)
    {  // rgb
        m_internalFormat = GL_RGB8;
        m_dataFormat = GL_RGB;
    }
    else
    {
        NAMICA_CORE_ASSERT(false, "OpenGLTexture2D 加载纹理不支持通道数为:{}", channels);
        return;
    }

    // opengl 创建texture对象
    glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
    createTextureStorage();
    // 上传纹理缓冲区数据
    glTextureSubImage2D(
        m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
    // 上传完毕后, data数据可以清理
    stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
    glDeleteTextures(1, &m_rendererID);
}

uint32_t OpenGLTexture2D::getWidth() const
{
    return m_width;
}

uint32_t OpenGLTexture2D::getHeight() const
{
    return m_height;
}

void OpenGLTexture2D::bind(uint32_t _slot) const
{
    glBindTextureUnit(_slot, m_rendererID);  // 将纹理数据绑定到slot插槽
}

void OpenGLTexture2D::setData(void* _data, uint32_t _size)
{
    NAMICA_CORE_ASSERT((_size == m_width * m_height * (m_dataFormat == GL_RGBA ? 4 : 3)),
                       "当前纹理设置的数据和宽度高度不匹配");

    glTextureSubImage2D(
        m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, _data);
}

bool OpenGLTexture2D::isEqual(Texture const& _other)
{
    return ((OpenGLTexture2D const&)_other).m_rendererID == m_rendererID;
}

uint32_t OpenGLTexture2D::getRendererId()
{
    return m_rendererID;
}

}  // namespace Namica