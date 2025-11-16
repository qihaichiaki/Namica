#pragma once

#include "namica/renderer/VertexBuffer.h"

namespace Namica
{

class OpenGLVertexBuffer final : public VertexBuffer
{
public:
    OpenGLVertexBuffer(float* _vertices, uint32_t _size);
    OpenGLVertexBuffer(uint32_t _size);
    ~OpenGLVertexBuffer();

    virtual void setData(void const* _data, uint32_t _size) override;
    virtual void setLayout(VertexLayout const& _vertexLayout) override;
    virtual VertexLayout const& getLayout() const noexcept override;

private:
    uint32_t m_rendererID;
    VertexLayout m_vertexLayout;

    friend class OpenGLVertexArray;
};

}  // namespace Namica