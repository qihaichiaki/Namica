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
};

}  // namespace Namica