#include "platform/opengl/OpenGLVertexBuffer.h"

#include <glad/glad.h>

namespace Namica
{

OpenGLVertexBuffer::OpenGLVertexBuffer(float* _vertices, uint32_t _size)
{
    // gl创建缓冲区
    glCreateBuffers(1, &m_rendererID);
    // gl绑定当前缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    // gl上传静态顶点缓冲区数据
    glBufferData(GL_ARRAY_BUFFER, _size, _vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t _size)
{
    // gl创建缓冲区
    glCreateBuffers(1, &m_rendererID);
    // gl绑定当前缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    // gl上传动态顶点缓冲区数据
    glBufferData(GL_ARRAY_BUFFER, _size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    // gl删除当前顶点缓冲区
    glDeleteBuffers(1, &m_rendererID);
}

void OpenGLVertexBuffer::setData(void const* _data, uint32_t _size)
{
    // gl绑定当前缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    // gl上传动态顶点缓冲区数据
    glBufferSubData(GL_ARRAY_BUFFER, 0, _size, _data);
}

void OpenGLVertexBuffer::setLayout(VertexLayout const& _vertexLayout)
{
    m_vertexLayout = _vertexLayout;
}

VertexLayout const& OpenGLVertexBuffer::getLayout() const noexcept
{
    return m_vertexLayout;
}

}  // namespace Namica