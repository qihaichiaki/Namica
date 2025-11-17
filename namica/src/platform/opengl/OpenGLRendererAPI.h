#pragma once

#include "namica/renderer/RendererAPI.h"

namespace Namica
{

class OpenGLRendererAPI final : public RendererAPI
{
public:
    virtual void init() override;
    virtual void setClearColor(glm::vec4 const& _clearColor) override;
    virtual void clear() override;
    virtual void drawIndexed(Ref<VertexArray> const& _vertexArray,
                             Ref<Shader> const& _shader) override;
    virtual void drawIndexed(Ref<VertexArray> const& _vertexArray,
                             Ref<Shader> const& _shader,
                             uint32_t _indexCount) override;
    virtual void drawLines(Ref<VertexArray> const& _vertexArray,
                           Ref<Shader> const& _shader,
                           uint32_t _vertexCount) override;
    virtual void setLineWidth(float _width) override;
    virtual void updateViewport(uint32_t _width, uint32_t _height) override;
};

}  // namespace Namica