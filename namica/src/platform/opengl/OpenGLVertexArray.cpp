#include "platform/opengl/OpenGLVertexArray.h"
#include "namica/core/Log.h"

#include <glad/glad.h>

namespace Namica
{

// shader type -> gl type
static GLenum shaderDataTypeToOpenGLBaseType(ShaderDataType _type)
{
    switch (_type)
    {
        case ShaderDataType::Float:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4:
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
            return GL_FLOAT;
        case ShaderDataType::Bool:
            return GL_BOOL;
        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
            return GL_INT;
        default:
            NAMICA_CORE_ASSERT(false);
            return 0;
    }
}

OpenGLVertexArray::OpenGLVertexArray()
{
    glCreateVertexArrays(1, &m_rendererID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
    glDeleteVertexArrays(1, &m_rendererID);
}

void OpenGLVertexArray::setVertexBuffer(Ref<VertexBuffer> const& _vertexBuffer)
{
    m_vertextBuffer = refCast<OpenGLVertexBuffer>(_vertexBuffer);
    VertexLayout const& vertexLayout{m_vertextBuffer->getLayout()};
    // VertexBuffer添加进来时, 需要将顶点元素布局设置完成, 不可为空
    NAMICA_CORE_ASSERT(!vertexLayout.empty());

    // 绑定当前顶点数组对象
    glBindVertexArray(m_rendererID);
    // 绑定设置的顶点缓冲区对象
    glBindBuffer(GL_ARRAY_BUFFER, m_vertextBuffer->m_rendererID);

    // 根据顶点缓冲区设置的顶点元素布局设置opengl布局
    int index{0};
    for (auto const& element : vertexLayout)
    {
        switch (element.type)
        {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index++,
                                      element.getComponentCount(),
                                      shaderDataTypeToOpenGLBaseType(element.type),
                                      element.normalized ? GL_TRUE : GL_FALSE,
                                      vertexLayout.getStride(),
                                      (void*)((uint64_t)element.offset));
                break;
            case ShaderDataType::Bool:
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
                glEnableVertexAttribArray(index);
                glVertexAttribIPointer(index++,
                                       element.getComponentCount(),
                                       shaderDataTypeToOpenGLBaseType(element.type),
                                       vertexLayout.getStride(),
                                       (void*)((uint64_t)element.offset));
                break;
            default:
                break;
        }
    }
}

void OpenGLVertexArray::setIndexBuffer(Ref<IndexBuffer> const& _indexBuffer)
{
    m_indexBuffer = refCast<OpenGLIndexBuffer>(_indexBuffer);

    glBindVertexArray(m_rendererID);
    // 绑定设置的索引缓冲区对象
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer->m_rendererID);
}

}  // namespace Namica