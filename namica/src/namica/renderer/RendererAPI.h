#pragma once

#include "namica/core/Memory.h"
#include <glm/glm.hpp>

namespace Namica
{

class VertexArray;
class Shader;
class RendererAPI
{
public:
    virtual ~RendererAPI() = default;

    virtual void init() = 0;
    virtual void setClearColor(glm::vec4 const& _clearColor) = 0;
    virtual void clear() = 0;
    virtual void drawIndexed(Ref<VertexArray> const& _vertexArray, Ref<Shader> const& _shader) = 0;
    virtual void drawIndexed(Ref<VertexArray> const& _vertexArray,
                             Ref<Shader> const& _shader,
                             uint32_t _indexCount) = 0;
    virtual void drawLines(Ref<VertexArray> const& _vertexArray,
                           Ref<Shader> const& _shader,
                           uint32_t _vertexCount) = 0;
    virtual void setLineWidth(float _width) = 0;
    virtual void updateViewport(uint32_t _width, uint32_t _height) = 0;
};

};  // namespace Namica