#include "platform/opengl/OpenGLIndexBuffer.h"

#include <glad/glad.h>

namespace Namica
{
OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t const* _indices, uint32_t _count) : m_count(_count)
{
    // gl创建缓冲区
    glCreateBuffers(1, &m_rendererID);
    // gl绑定索引缓冲区
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
    // 上传索引数据
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(uint32_t), _indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &m_rendererID);
}

uint32_t OpenGLIndexBuffer::getCount() const noexcept
{
    return m_count;
}

}  // namespace Namica