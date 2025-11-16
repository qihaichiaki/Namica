#include "namica/renderer/VertexBuffer.h"
#include "namica/core/Log.h"
#include "namica/renderer/RendererCommand.h"

#include "platform/opengl/OpenGLVertexBuffer.h"

namespace Namica
{

static inline uint32_t shaderDataTypeSize(ShaderDataType _type)
{
    switch (_type)
    {
        case ShaderDataType::Float:
        case ShaderDataType::Int:
            return 4;
        case ShaderDataType::Float2:
        case ShaderDataType::Int2:
            return 8;
        case ShaderDataType::Float3:
        case ShaderDataType::Int3:
            return 12;
        case ShaderDataType::Float4:
        case ShaderDataType::Int4:
            return 16;
        case ShaderDataType::Mat3:
            return 4 * 3 * 3;
        case ShaderDataType::Mat4:
            return 4 * 4 * 4;
        case ShaderDataType::Bool:
            return 1;
        default:
            NAMICA_CORE_ASSERT(false);
            return 0;
    }
}

VertexElement::VertexElement(ShaderDataType _type, char const* _name, bool _normalized)
    : type(_type), name(_name), offset(0), size(shaderDataTypeSize(_type)), normalized(_normalized)
{
}

uint32_t VertexElement::getComponentCount() const noexcept
{
    switch (type)
    {
        case ShaderDataType::Float:
        case ShaderDataType::Int:
        case ShaderDataType::Bool:
            return 1;
        case ShaderDataType::Float2:
        case ShaderDataType::Int2:
            return 2;
        case ShaderDataType::Float3:
        case ShaderDataType::Int3:
            return 3;
        case ShaderDataType::Float4:
        case ShaderDataType::Int4:
            return 4;
        case ShaderDataType::Mat3:
            return 3 * 3;
        case ShaderDataType::Mat4:
            return 4 * 4;
        default:
            NAMICA_CORE_ASSERT(false);
            return 0;
    }
}

VertexLayout::VertexLayout(std::initializer_list<VertexElement> const& _elements)
    : m_elements(_elements), m_stride(0)
{
    for (auto& element : m_elements)
    {
        element.offset = m_stride;
        m_stride += element.size;
    }
}

std::vector<VertexElement>::iterator VertexLayout::begin()
{
    return m_elements.begin();
}

std::vector<VertexElement>::iterator VertexLayout::end()
{
    return m_elements.end();
}

std::vector<VertexElement>::const_iterator VertexLayout::begin() const
{
    return m_elements.begin();
}

std::vector<VertexElement>::const_iterator VertexLayout::end() const
{
    return m_elements.end();
}

bool VertexLayout::empty() const
{
    return m_elements.empty();
}
uint32_t VertexLayout::getStride() const
{
    return m_stride;
}

Ref<VertexBuffer> VertexBuffer::create(float* _vertices, uint32_t _size)
{
    switch (RendererCommand::getRendererAPIType())
    {
        case RendererAPIType::OpenGL:
            return createRef<OpenGLVertexBuffer>(_vertices, _size);
        default:
            NAMICA_CORE_ASSERT(false);
            return nullptr;
    }
}

Ref<VertexBuffer> VertexBuffer::create(uint32_t _size)
{
    switch (RendererCommand::getRendererAPIType())
    {
        case RendererAPIType::OpenGL:
            return createRef<OpenGLVertexBuffer>(_size);
        default:
            NAMICA_CORE_ASSERT(false);
            return nullptr;
    }
}

}  // namespace Namica