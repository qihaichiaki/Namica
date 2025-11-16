#pragma once

#include "namica/renderer/IndexBuffer.h"

namespace Namica
{

class OpenGLIndexBuffer final : public IndexBuffer
{
public:
    OpenGLIndexBuffer(uint32_t const* _indices, uint32_t _count);
    ~OpenGLIndexBuffer();

    virtual uint32_t getCount() const noexcept override;

private:
    uint32_t m_rendererID;
    uint32_t m_count;

    friend class OpenGLVertexArray;
};

}  // namespace Namica