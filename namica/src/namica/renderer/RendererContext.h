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

    /**
     * @brief 获取当前图形上下文的原始指针
     */
    virtual void* getCurrentNativeHandle() = 0;

    /**
     * @brief 设置图形原始句柄指针为当前图形上下文
     *
     */
    virtual void makeCurrent(void*) = 0;
};

}  // namespace Namica