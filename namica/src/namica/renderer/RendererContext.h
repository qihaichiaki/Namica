#pragma once

namespace Namica
{

class RendererContext
{
public:
    RendererContext() = default;
    virtual ~RendererContext() = default;

    /**
     * @brief 图形上下文初始化
     */
    virtual void init() = 0;
};

}  // namespace Namica