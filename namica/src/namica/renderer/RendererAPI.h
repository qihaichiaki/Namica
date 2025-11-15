#pragma once

#include <glm/glm.hpp>

namespace Namica
{

class RendererAPI
{
public:
    virtual ~RendererAPI() = default;

    virtual void init() = 0;
    virtual void setClearColor(glm::vec4 const& _clearColor) = 0;
    virtual void clear() = 0;
};

};  // namespace Namica