#include "platform/opengl/OpenGLUniformBuffer.h"
#include <glad/glad.h>

namespace Namica
{

OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t _size, uint32_t _binding)
{
    glCreateBuffers(1, &m_rendererID);
    glNamedBufferData(m_rendererID, _size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, _binding, m_rendererID);
}

OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
    glDeleteBuffers(1, &m_rendererID);
}

void OpenGLUniformBuffer::setData(void const* _data, uint32_t _size, uint32_t _offset)
{
    glNamedBufferSubData(m_rendererID, _offset, _size, _data);
}

}  // namespace Namica