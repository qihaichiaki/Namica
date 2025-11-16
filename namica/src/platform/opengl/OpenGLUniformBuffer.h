#pragma once

#include "namica/renderer/UniformBuffer.h"

namespace Namica
{

class OpenGLUniformBuffer final : public UniformBuffer
{
public:
    OpenGLUniformBuffer(uint32_t _size, uint32_t _binding);
    ~OpenGLUniformBuffer();

    virtual void setData(void const* _data, uint32_t _size, uint32_t _offset = 0) override;

private:
    uint32_t m_rendererID;
};

}  // namespace Namica