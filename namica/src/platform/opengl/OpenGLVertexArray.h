#pragma once

#include "namica/renderer/VertexArray.h"
#include "platform/opengl/OpenGLVertexBuffer.h"
#include "platform/opengl/OpenGLIndexBuffer.h"

namespace Namica
{

class OpenGLVertexArray final : public VertexArray
{
public:
    OpenGLVertexArray();
    ~OpenGLVertexArray();

    virtual void setVertexBuffer(Ref<VertexBuffer> const& _vertexBuffer) override;
    virtual void setIndexBuffer(Ref<IndexBuffer> const& _indexBuffer) override;

private:
    uint32_t m_rendererID;
    Ref<OpenGLVertexBuffer> m_vertextBuffer;
    Ref<OpenGLIndexBuffer> m_indexBuffer;
};

}  // namespace Namica